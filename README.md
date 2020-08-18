## linux kernel repository

### linux-surface branches

`v${kernelver}-surface-devel`:
- tracking upstream [linux-surface/kernel](linux-surface/kernel) development branches, if exist in my repo.

`v${kernelver}-surface`:
- tracking upstream [linux-surface/kernel](linux-surface/kernel) patch generation branches, if exist in my repo. Patches will be created by cherry-picking commits from `-devel` branches and squashing.

### chromiumos kernel branches

`chromeos-${kernelver}`:
- tracking upstream [chromiumos kernel](https://chromium.googlesource.com/chromiumos/third_party/kernel/) branches, if exist in my repo.

`chromeos-${kernelver}-surface-devel`:
- Merging v${kernelver}-surface-devel commits on top of chromeos-${kernelver}.

`chromeos-${kernelver}-surface`:
- Patch generation branches for chromiumos kernel. Patches will be created by cherry-picking commits from `-devel` branches and squashing.

`chromeos-${kernelver}-surface-devel+k5`:
- chromeos-${kernelver}-surface-devel plus some patches needed to run well on chromiumos/brunch.

### chromiumos kernel tags

`chromeos-${base_chromiumos_kernel_describe_ver}+surface+k5`:
- Where `base_chromiumos_kernel_describe_ver=$(git describe chromeos-${kernelver})`. `chromeos-${kernelver}-surface` patches and `chromeos-${kernelver}-surface-devel+k5` (without `-surface-devel` commits) are applied on top of upstream chromiumos kernel tree.

For example:
```bash
$ git describe chromeos-5.4
v5.4.58-7455-g0a7a035a76b8

$ git checkout chromeos-5.4 -b chromeos-v5.4.58-7455-g0a7a035a76b8+surface+k5
$ git merge chromeos-5.4-surface

# because I want to apply k5 patches on top of `-surface` patches,
# but `chromeos-5.4-surface-devel+k5` branch also contains `-surface-devel`
# patches. So, cherry-pick patches between them.
$ git cherry-pick chromeos-5.4-surface-devel..chromeos-5.4-surface-devel+k5
```

### Cherry Trail related branches (mainly for atomisp stuff leaning)

Android-IA kernel trees for Cherry Trail, applied kernel quilts from Intel:
- [ProductionKernelQuilts/cht-m1stable-2016_ww31](https://github.com/kitakar5525/linux-kernel/tree/ProductionKernelQuilts/cht-m1stable-2016_ww31)
- [ProductionKernelQuilts/cht-m1stable-2016_ww19](https://github.com/kitakar5525/linux-kernel/tree/ProductionKernelQuilts/cht-m1stable-2016_ww19)

Android-IA vendor kernel trees:
- [lenovo_yb1_x90f_l_osc_201803](https://github.com/kitakar5525/linux-kernel/tree/lenovo_yb1_x90f_l_osc_201803)
- [xiaomi-mipad2/latte-l-oss](https://github.com/kitakar5525/linux-kernel/tree/xiaomi-mipad2/latte-l-oss)
