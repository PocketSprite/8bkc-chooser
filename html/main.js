var xhr = j();
var $ub;
var sort_column, sort_direction;

var params;
(window.onpopstate = function () {
    var match,
        pl     = /\+/g,  // Regex for replacing addition symbol with a space
        search = /([^&=]+)=?([^&]*)/g,
        decode = function (s) { return decodeURIComponent(s.replace(pl, " ")); },
        query  = window.location.search.substring(1);
    params = {};
    while (match = search.exec(query))
       params[decode(match[1])] = decode(match[2]);
})();

function get_sort_from_url(){
	// We're looking for the following:
	// sc=[name, size]
	// sd=[a, d]
	if (params["sc"]) {
		sort_column = params["sc"];
		sort_direction = params["sd"] || "a";
	}
}

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
			if (sort_column && sort_direction) {
				obj.files.sort(function (a, b) {
					if (sort_direction == "a") {
						// ascending
						return b[sort_column] < a[sort_column] ? 1 : -1;
					} else {
						// descending
						return b[sort_column] < a[sort_column] ? -1 : 1;
					}
				});
			}
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
function setupSortBinds () {
	var sortButtons = document.querySelectorAll(".sort");

	function handleSortClick(el) {
		// Get the column and direction from the classes
		// This is a bit of a hack, but it'll do.
		var classes = el.getAttribute("class").split(" ").slice(1,3);
		var sc = classes[0];
		var sd = classes[1];

		var queryParameter = "?sc=" + sc + "&sd=" + sd;
		window.history.pushState({}, "", queryParameter);
		sort_column = sc;
		sort_direction = sd;
		triggerReload();
	}
	for (var i=0; i<sortButtons.length; i++) {
		sortButtons[i].addEventListener('click', function () {
			handleSortClick(this);
		});
	}
}
window.onload = function() {
	$ub = $("#ub");
	get_sort_from_url();
	setupSortBinds();
	triggerReload();
}