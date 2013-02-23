#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

//TESTESTTEST
//////////
/*
asmlinkage long sys_set_colors()
{
	printk("Hello from set color!\n");
}

asmlinkage long sys_get_colors(int i)
{
	printk("Hello from get color with number %d\n",i);
}*/
SYSCALL_DEFINE0(set_colors)
{
	printk("Hello from set color!\n");
	return 0;
}
SYSCALL_DEFINE1(get_colors, int, i)
{
	printk("Hello from set color!%d\n",i);
	return 0;
}

