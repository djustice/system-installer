# Maintainer: Drake Justice <djustice@system-linux.com>
pkgname=system-installer
pkgver=0.0.1
pkgrel=1
pkgdesc="A Graphical Installer for System Linux"
arch=('x86_64')
url="https://github.com/djustice/system-installer"
license=('GPL3')
depends=('gcc-libs')
makedepends=('cmake' 'make' 'gcc')
source=("${pkgname}-${pkgver}.tar.gz")
sha256sums=('SKIP')

build() {
  cd "${srcdir}/${pkgname}"
  cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr
  cmake --build build
}

package() {
  cd "${srcdir}/${pkgname}"
  DESTDIR="${pkgdir}" cmake --install build
}
