#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/rcupdate.h>
#include <linux/fdtable.h>
#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/dcache.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual BSD/GPL");

static int current_init(void)
{
    struct path pwd; 
    char *name = NULL;


    struct task_struct *p = NULL;

    unsigned long fsOffset;
    unsigned long dentryOffset;
    unsigned long parentOffset;
    unsigned long nameOffset;



    p = current;

    if (p!= NULL) {
        fsOffset = (unsigned long)(&(p->fs)) - (unsigned long) (p);
        dentryOffset = (unsigned long)(&(p->fs->pwd.dentry)) - (unsigned long)(p->fs);
        parentOffset = (unsigned long)(&(p->fs->pwd.dentry->d_parent)) - (unsigned long)(p->fs->pwd.dentry);
        nameOffset = (unsigned long)(&(p->fs->pwd.dentry->d_iname)) - (unsigned long)(p->fs->pwd.dentry);

        printk(KERN_ALERT "fs_struct = 0x%x;\n",(unsigned int) fsOffset);
        printk(KERN_ALERT "dentry = 0x%x;\n",(unsigned int) dentryOffset);
        printk(KERN_ALERT "d_parent = 0x%x;\n",(unsigned int) parentOffset);
        printk(KERN_ALERT "d_iname = 0x%x;\n",(unsigned int) nameOffset);
    }

    pwd = current->fs->pwd;
    name = pwd.dentry->d_iname;
    printk("%s\n", name);
    name = pwd.dentry->d_parent->d_iname;
    printk("%s\n", name);
    return 0;
}

static void current_exit(void)
{
    printk(KERN_ALERT "Goodbye cruel world\n");
}

module_init(current_init);
module_exit(current_exit);
