# Maintainer: Your Name <youremail@example.com>
pkgname=tmp
pkgver=1.0.0
pkgrel=1
pkgdesc="A simple task manager application (Task Manager Plus) to track daily task completion streaks."
arch=('x86_64')
url="https://example.com"  # Set to your project URL or GitHub repo
depends=('cjson' 'gcc' 'make')  # Dependencies
source=("task_manager.c")  # List of source files
md5sums=('SKIP')  # Set this to the checksum of your source file, if available

build() {
    cd "$srcdir"
    gcc -o tmp task_manager.c -I/usr/include/cjson -L/usr/lib -lcjson
}

package() {
    cd "$srcdir"
    install -Dm755 tmp "$pkgdir/usr/bin/tmp"
    # Optional: add documentation

}

