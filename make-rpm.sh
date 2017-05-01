#!/bin/sh
# export the tarball

# update the .spec file with the version number

# upload both files
echo "[general]" > ~/.oscrc
echo "apiurl = https://api.opensuse.org" >> ~/.oscrc
echo "[https://api.opensuse.org]" >> ~/.oscrc
echo "user = qmfrederik" >> ~/.oscrc
echo "pass = $GPG_PASSPHRASE" >> ~/.oscrc

cd $HOME/build/CoreCompat
mkdir osc
cd osc
osc checkout home:qmfrederik
cd home\:qmfrederik/libgdiplus
osc rm *
cd $HOME/build/CoreCompat/libgdiplus
git archive --format tar.gz -o ../osc/home\:qmfrederik/libgdiplus/libgdiplus-${LIBGDIPLUS_VERSION_PREFIX}.$TRAVIS_BUILD_NUMBER.tar.gz --prefix libgdiplus-${LIBGDIPLUS_VERSION_PREFIX}.$TRAVIS_BUILD_NUMBER/ HEAD
cp libgdiplus0.spec ../osc/home\:qmfrederik/libgdiplus/
cd $HOME/build/CoreCompat/osc/home\:qmfrederik/libgdiplus
sed -i "s/4.2/${LIBGDIPLUS_VERSION_PREFIX}.${TRAVIS_BUILD_NUMBER}/" libgdiplus0.spec
sed -i "s/tar.bz2/tar.gz/" libgdiplus0.spec
osc add *
osc commit -m "Update for Travis CI build $TRAVIS_BUILD_NUMBER"
