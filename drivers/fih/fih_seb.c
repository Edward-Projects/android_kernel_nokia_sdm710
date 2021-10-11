#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/io.h>
#include <linux/of_device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/file.h>
#include <linux/uaccess.h>

static char enabled[16];
static char serialno[16];
static char tampered[16];
static char unlocked[16];

static int fih_seb_proc_read_enabled(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", enabled);
	return 0;
}

static int fih_seb_proc_open_enabled(struct inode *inode, struct file *file)
{
	return single_open(file, fih_seb_proc_read_enabled, NULL);
}

static const struct file_operations fih_seb_fops_enabled = {
	.open    = fih_seb_proc_open_enabled,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

static int fih_seb_proc_read_serialno(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", serialno);
	return 0;
}

static int fih_seb_proc_open_serialno(struct inode *inode, struct file *file)
{
	return single_open(file, fih_seb_proc_read_serialno, NULL);
}

static const struct file_operations fih_seb_fops_serialno = {
	.open    = fih_seb_proc_open_serialno,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

static int fih_seb_proc_read_tampered(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", tampered);
	return 0;
}

static int fih_seb_proc_open_tampered(struct inode *inode, struct file *file)
{
	return single_open(file, fih_seb_proc_read_tampered, NULL);
}

static const struct file_operations fih_seb_fops_tampered = {
	.open    = fih_seb_proc_open_tampered,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

static int fih_seb_proc_read_unlocked(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", unlocked);
	return 0;
}

static int fih_seb_proc_open_unlocked(struct inode *inode, struct file *file)
{
	return single_open(file, fih_seb_proc_read_unlocked, NULL);
}

static const struct file_operations fih_seb_fops_unlocked = {
	.open    = fih_seb_proc_open_unlocked,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

static int fih_seb_property(struct platform_device *pdev)
{
	int rc = 0;
	static const char *p_chr;

	p_chr = of_get_property(pdev->dev.of_node, "fih-seb,enabled", NULL);
	if (!p_chr) {
		pr_info("%s:%d, enabled not specified\n", __func__, __LINE__);
	} else {
		snprintf(enabled, sizeof(enabled), "%s", p_chr);
		pr_info("%s: enabled = %s\n", __func__, enabled);
	}

	p_chr = of_get_property(pdev->dev.of_node, "fih-seb,serialno", NULL);
	if (!p_chr) {
		pr_info("%s:%d, serialno not specified\n", __func__, __LINE__);
	} else {
		snprintf(serialno, sizeof(serialno), "%s", p_chr);
		pr_info("%s: serialno = %s\n", __func__, serialno);
	}

	p_chr = of_get_property(pdev->dev.of_node, "fih-seb,tampered", NULL);
	if (!p_chr) {
		pr_info("%s:%d, tampered not specified\n", __func__, __LINE__);
	} else {
		snprintf(tampered, sizeof(tampered), "%s", p_chr);
		pr_info("%s: tampered = %s\n", __func__, tampered);
	}

	p_chr = of_get_property(pdev->dev.of_node, "fih-seb,unlocked", NULL);
	if (!p_chr) {
		pr_info("%s:%d, unlocked not specified\n", __func__, __LINE__);
	} else {
		snprintf(unlocked, sizeof(unlocked), "%s", p_chr);
		pr_info("%s: unlocked = %s\n", __func__, unlocked);
	}

	return rc;
}

static int fih_seb_probe(struct platform_device *pdev)
{
	int rc = 0;

	if (!pdev || !pdev->dev.of_node) {
		pr_err("%s: Unable to load device node\n", __func__);
		return -ENOTSUPP;
	}

	rc = fih_seb_property(pdev);
	if (rc) {
		pr_err("%s Unable to set property\n", __func__);
		return rc;
	}

	proc_mkdir("secboot", NULL);
	proc_create("secboot/enabled",  0, NULL, &fih_seb_fops_enabled);
	proc_create("secboot/serialno", 0, NULL, &fih_seb_fops_serialno);
	proc_create("secboot/tampered", 0, NULL, &fih_seb_fops_tampered);
	proc_create("secboot/unlocked", 0, NULL, &fih_seb_fops_unlocked);

	return rc;
}

static int fih_seb_remove(struct platform_device *pdev)
{
	remove_proc_entry ("secboot/unlocked", NULL);
	remove_proc_entry ("secboot/tampered", NULL);
	remove_proc_entry ("secboot/serialno", NULL);
	remove_proc_entry ("secboot/enabled",  NULL);

	return 0;
}

static const struct of_device_id fih_seb_dt_match[] = {
	{.compatible = "fih_seb"},
	{}
};
MODULE_DEVICE_TABLE(of, fih_seb_dt_match);

static struct platform_driver fih_seb_driver = {
	.probe = fih_seb_probe,
	.remove = fih_seb_remove,
	.shutdown = NULL,
	.driver = {
		.name = "fih_seb",
		.of_match_table = fih_seb_dt_match,
	},
};

static int __init fih_seb_init(void)
{
	int ret;

	ret = platform_driver_register(&fih_seb_driver);
	if (ret) {
		pr_err("%s: failed!\n", __func__);
		return ret;
	}

	return ret;
}
module_init(fih_seb_init);

static void __exit fih_seb_exit(void)
{
	platform_driver_unregister(&fih_seb_driver);
}
module_exit(fih_seb_exit);
