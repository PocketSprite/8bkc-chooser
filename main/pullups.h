#ifndef PULLUPS_H
#define PULLUPS_H

void gpio_enable_pull_up(gpio_num_t gpio_num);
void gpio_disable_pull_up(gpio_num_t gpio_num);
void gpio_enable_pull_down(gpio_num_t gpio_num);
void gpio_disable_pull_down(gpio_num_t gpio_num);

#endif