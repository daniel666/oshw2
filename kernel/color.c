#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/cred.h>
//TESTESTTEST
/*
asmlinkage long sys_set_colors()
{
	printk("Hello from set color!\n");
}

asmlinkage long sys_get_colors(int i)
{
	printk("Hello from get color with number %d\n",i);
}*/
asmlinkage long sys_set_colors(void)
{
	printk(KERN_DEBUG "Hello from set color printk!\n");
	return 0;
}

/*SYSCALL_DEFINE0(set_colors)
{
	printk(KERN_DEBUG "Hello from set color printk!\n");
	return 0;
}*/
SYSCALL_DEFINE0(get_colors)
{
	printk("Hello from set color!\n");
	return 0;
}


/*
 SYSCALL_DEFINE4(set_colors, int, nr_pids, pid_t *, pids, u_int16_t *, colors, int *, retval)
 {
      pid_t  kpids[nr_pids];
      u_int16_t kcolors[nr_pids];
      int   kretval[nr_pids];
      int   i;
      struct task_struct* task, sibling_entry, child_entry;
      struct list_head* sibling, child;
      tgid  tgid;

      //if not root, return error
      if(current()->uid) {return -EACCES}
      if(copy_from_user(kpids, pids, sizeof(nr_pids)))
            return -EFAULT;
      if(copy_from_user(kcolors, colors, sizeof(nr_pid)))
            return -EFAULT;
      int(i=0;i<nr_pids;i++){
            //need to lock pid???
            task = find_process_by_pid(kpids[i]);
            if(!task){
                   kretval[i]=-EINVAL;
                   continue;
            }
            tgid = task->tgid;
            struct task_struct * thread_task;
            while_each_thread(thread_task, task){
                  unsigned long flags=0;
                  thread_task->tasklist_lock = RW_LOCK_UNLOCKED;
                  write_lock_irqsave(&thread_task->tasklist_lock, flags);
                  thread_task->color = kcolors[i];
                  kretval[i] = 0;
                  write_unlock_irqstore(&thread_task->tasklist_lock, flags);
            }
-------------------------------------------------------------------------------------------------------------------------------
Maybe I don't need this part

            list_for_each(child,&task->children){
                  if((child_entry=list_entry(child, struct task_struct, children))->tgid==tgid){
                        unsigned long flags=0;
                        child_enry->tasklist_lock = RW_LOCK_UNLOCKED;
                        write_lock_irqsave(&child_entry->tasklist_lock, flags);
                        child_entry->color = colors[i];
                        write_unlock_irqsotre(&child_entry->tasklist_lock, flags);
                  }
            }

            list_for_each(sibling, &task->sibling){
                  if((sibling_entry=list_entry(sibling, struct task_struct, sibling))->tgid == tgid){
                        list_for_each(child,sibling_entry){
                              if((child_entry=list_entry(child, struct task_struct, children))->tgid==tgid){
                                    unsigned long flags=0;
                                    child_enry->tasklist_lock = RW_LOCK_UNLOCKED;
                                    write_lock_irqsave(&child_entry->tasklist_lock, flags);
                                    child_entry->color = colors[i];
                                    write_unlock_irqstore(&child_entry->tasklist_lock, flags);
                              }
                        }
                  }
            }
 ------------------------------------------------------------------------------------------------------------------------------------
      }
      if(copy_to_user(retval,kretval, sizeof(nr_pids)))
            return -EFAULT;
}

SYSCALL_DEFINE4(get_colors, int, nr_pids, pid_t *, pids, u_int16_t *, colors, int *, retval)
{
      pid_t  kpids[nr_pids];
      u_int16_t kcolors[nr_pids];
      int   kretval[nr_pids];
      int   i;
      struct task_struct* task, sibling_entry, child_entry;
      struct list_head* sibling, child;
      tgid  tgid;

      if(copy_from_user(kpids, pids, sizeof(nr_pids)))
            return -EFAULT;
      if(copy_from_user(kcolors, colors, sizeof(nr_pid)))
            return -EFAULT;

      for(int i=0; i<nr_pids; i++){
            task = find_process_by_pid(kpids[i]);
            if(!task){
                   kretval[i]=-EINVAL;
                   continue;
            }
            unsigned long flags;
            read_lock_irqsave(&task->tasklist_lock, flags);
            kcolors[i] = task->color;
            kretval[i] = 0;
            read_unlock_irqstore(&task->tasklist_lock, flags);
      }
      if(copy_to_user(retval,kretval, sizeof(nr_pids)))
            return -EFAULT;
}
*/
