{pkgs}: {
  deps = [
    pkgs.cmake
    pkgs.msbuild
    pkgs.pkg-config-unwrapped
    pkgs.vcpkg
  ];
}
