var xhr = j();
var $ub;

//Sets the progress bar to a specific level. Amt is between 0 and 1.
function setProgress(amt) {
	$("#progressbarinner").style.width = String(amt * $("#progressbar").clientWidth) + "px";
}

//Called when the submit button is called.
function doUpgrade() {
	//Grab the file, see if it's a real file.
	var f = $("#file").files[0];
	var r = $("#remark");
	r.style.display = "none";
	if (typeof f == 'undefined') {
		r.style.display = "block";
		r.innerHTML = "Can't read file!";
		return
	}
	var name = $("#file").value
	name = name.replace(/^.*[\\\/]/, '')
	xhr.open("POST", "upload.cgi?name=" + encodeURIComponent(name));
	xhr.onreadystatechange = function() {
		if (xhr.readyState == 4 && ((xhr.status >= 200 && xhr.status < 300) || xhr.status == 400)) {
			//No more fluid progress bar
			$("#progressbarinner").style.transition="0s";
			setProgress(1);
			if (xhr.responseText != "") {
				alert("Error: " + xhr.responseText);
				$ub.value = "Error!";
			} else {
				triggerReload();
			}
		}
	}
	//If the webbrowser enables it, make progress bar show progress.
	if (typeof xhr.upload.onprogress != 'undefined') {
		xhr.upload.onprogress = function(e) {
			setProgress(e.loaded / e.total);
			$ub.value = "Uploading…";
		}
	}
	//Fluid progress bar
	$("#progressbarinner").style.transition=".2s";
	//Upload the file
	xhr.send(f);
	return false;
}

function triggerReload() {
	$ub.value = "Upload!";
	xhr.open("GET", "fileidx.cgi");
	xhr.onreadystatechange = function() {
		if (xhr.readyState == 4 && xhr.status >= 200 && xhr.status < 300) {
			var obj = JSON.parse(xhr.responseText);
			var table = $("#fileidx");
			while (table.rows.length != 1) table.deleteRow(1);
			for (var x = 0; x < obj.files.length; x++) {
				var row = table.insertRow(-1);
				var cName = row.insertCell(-1);
				var cSize = row.insertCell(-1);
				var cDelete = row.insertCell(-1);
				var na = document.createElement('a');
				na.setAttribute('href', 'download.cgi?idx=' + obj.files[x].index);
				na.appendChild(document.createTextNode(obj.files[x].name));
				cName.appendChild(na);
				cSize.appendChild(document.createTextNode(formatBytes(obj.files[x].size)));
				var a = document.createElement('a');
				a.setAttribute('href', 'delete.cgi?idx=' + obj.files[x].index);
				a.appendChild(document.createTextNode('✖'));
				cDelete.appendChild(a);
			}
			$("#free").firstChild.data=formatBytes(obj.free);
			setProgress(0);
		}
	}
	xhr.send();
}

function formatBytes(size){
	if (size >= (1024 * 1024)) {
		return parseInt(size / (1024 * 1024)) + " MiB";
	} else {
		return parseInt(size / (1024)) + " KiB";
	}
}

window.onload = function() {
	$ub = $("#ub");
	triggerReload();
}