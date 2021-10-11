#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mutex.h>
#include "fih_recalibration_status.h"

#define FIH_PROC_SIZE  FIH_RECALIBRATION_STATUS_SIZE
static struct mutex lock;
static char fih_proc_recal_status[FIH_PROC_SIZE] = "0";

void fih_recalibration_status_setup(char *info)
{
	mutex_lock(&lock);
	snprintf(fih_proc_recal_status, sizeof(fih_proc_recal_status), "%s", info);
	mutex_unlock(&lock);
}

static int fih_proc_read_show(struct seq_file *m, void *v)
{
	mutex_lock(&lock);
	seq_printf(m, "%s\n", fih_proc_recal_status);
	mutex_unlock(&lock);
	return 0;
}

static int recal_status_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, fih_proc_read_show, NULL);
};

static struct file_operations recal_status_file_ops = {
	.owner   = THIS_MODULE,
	.open    = recal_status_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release
};

static int __init fih_proc_init(void)
{
	mutex_init(&lock);
	proc_create("arcsoft_recal_status", 0, NULL, &recal_status_file_ops);
	return (0);
}

static void __exit fih_proc_exit(void)
{
	remove_proc_entry ("arcsoft_recal_status", NULL);
}

module_init(fih_proc_init);
module_exit(fih_proc_exit);
