# Maintainer: Michael Stevenson <mikestevenson@tutamail.com>
pkgname=tdp
pkgver=1.0.0
pkgrel=1
pkgdesc="A simple todo application to track daily task completion and streaks."
arch=('x86_64')
url="bingo76.me"  # Set to your project URL or GitHub repo
depends=('cjson' 'gcc' 'make')  # Dependencies
source=("todoplus.c")  # List of source files
md5sums=('SKIP')  # Set this to the checksum of your source file, if available

build() {
    cd "$srcdir"
    gcc -o tdp todoplus.c -I/usr/include/cjson -L/usr/lib -lcjson
}

package() {
    cd "$srcdir"
    install -Dm755 tdp "$pkgdir/usr/bin/tdp"
}

