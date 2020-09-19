### memo: updating chromeos kernel tree

#### chromeos-4.19

```bash
# for the first time only
# git remote add chromeos-kernel https://chromium.googlesource.com/chromiumos/third_party/kernel
# git remote add linux-surface https://github.com/linux-surface/kernel
# cd ..; git remote add linux-surface https://github.com/linux-surface/kernel; cd -

git fetch chromeos-kernel
git fetch linux-surface
cd ../linux-surface; git pull; cd -

# generate tag name
TAG_KERNEL="chromeos-$(git describe chromeos-kernel/chromeos-4.19)+surface+k5"
echo $TAG_KERNEL

git checkout chromeos-4.19-surface-devel
git merge linux-surface/v4.19-surface-devel

git checkout chromeos-4.19-surface-devel+k5
git rebase chromeos-4.19-surface-devel

git checkout chromeos-4.19-surface
git reset --hard chromeos-kernel/chromeos-4.19
# Can't just merge v4.19-surface because it may have newer linux commits.
# So, apply patches from linux-surface/linux-surface repo.
git am -3 ../linux-surface/patches/4.19/*.patch
# Note_1: Resolve conflicts here. You may want to enable rerere:
# git config --global rerere.enabled true
# Note_2: The chromiumos kernel has NVMe commits that are equivalent to
# linux-surface's suspend.patch. So, you may want to skip this patch.

git checkout release/chromeos-4.19+surface+k5
git reset --hard chromeos-kernel/chromeos-4.19
git merge chromeos-4.19-surface --no-ff
# because I want to apply additional patches on top of `-surface` patches,
# but `chromeos-5.4-surface-devel+k5` branch also contains `-surface-devel`
# patches. So, cherry-pick patches between them.
git cherry-pick chromeos-4.19-surface-devel..chromeos-4.19-surface-devel+k5
git commit -s --allow-empty -F - <<EOF
memo: additional patches

cherry-picked patches from chromeos-4.19-surface-devel+k5

    $ git cherry-pick chromeos-4.19-surface-devel..chromeos-4.19-surface-devel+k5
EOF

git tag $TAG_KERNEL
git push origin $TAG_KERNEL
```

#### chromeos-5.4

```bash
# for the first time only
# git remote add chromeos-kernel https://chromium.googlesource.com/chromiumos/third_party/kernel
# git remote add linux-surface https://github.com/linux-surface/kernel
# cd ..; git remote add linux-surface https://github.com/linux-surface/kernel; cd -

git fetch chromeos-kernel
git fetch linux-surface
cd ../linux-surface; git pull; cd -

# generate tag name
TAG_KERNEL="chromeos-$(git describe chromeos-kernel/chromeos-5.4)+surface+k5"
echo $TAG_KERNEL

git checkout chromeos-5.4-surface-devel
git merge linux-surface/v5.4-surface-devel

git checkout chromeos-5.4-surface-devel+k5
git rebase chromeos-5.4-surface-devel

# The uapi version of ipts (to be used with iptsd)
git checkout chromeos-5.4-surface-devel-uapi_ipts
git rebase chromeos-5.4-surface-devel

git checkout chromeos-5.4-surface
git reset --hard chromeos-kernel/chromeos-5.4
# Can't just merge v5.4-surface because it may have newer linux commits.
# So, apply patches from linux-surface/linux-surface repo.
git am -3 ../linux-surface/patches/5.4/*.patch
# Note: Resolve conflicts here. You may want to enable rerere:
# git config --global rerere.enabled true

git checkout release/chromeos-5.4+surface+k5
git reset --hard chromeos-kernel/chromeos-5.4
git merge chromeos-5.4-surface --no-ff
# because I want to apply additional patches on top of `-surface` patches,
# but `chromeos-5.4-surface-devel+k5` branch also contains `-surface-devel`
# patches. So, cherry-pick patches between them.
git cherry-pick chromeos-5.4-surface-devel..chromeos-5.4-surface-devel+k5
git commit -s --allow-empty -F - <<EOF
memo: additional patches

cherry-picked patches from chromeos-5.4-surface-devel+k5

    $ git cherry-pick chromeos-5.4-surface-devel..chromeos-5.4-surface-devel+k5
EOF
# Switch to use the uapi version of ipts (to be used with iptsd)
git cherry-pick chromeos-5.4-surface-devel..chromeos-5.4-surface-devel-uapi_ipts
git commit -s --allow-empty -F - <<EOF
memo: uapi_ipts

Cherry-picked patches from chromeos-5.4-surface-devel-uapi_ipts:

    $ git cherry-pick chromeos-5.4-surface-devel..chromeos-5.4-surface-devel-uapi_ipts

These patches replaces current singletouch ipts with uapi singletouch
that works with iptsd.
EOF

git tag $TAG_KERNEL
git push origin $TAG_KERNEL
```
