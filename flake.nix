{
  description = "CAN-logger STM32 firmware development environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }: let
    system = "aarch64-darwin";  # use "x86_64-linux" on Linux
    pkgs = nixpkgs.legacyPackages.${system};
  in {
    devShells.${system}.default = pkgs.mkShell {
      name = "can-logger-dev";

      packages = [
        pkgs.gcc-arm-embedded
        pkgs.gnumake
        pkgs.git
        pkgs.openocd
      ];

      shellHook = ''
        echo "CAN-logger dev environment ready."
        echo "  libopencm3: $(dirname $PWD)/libopencm3"
        echo ""
        echo "First time setup:"
        echo "  1. git submodule update --init --recursive"
        echo "  2. make -C libopencm3"
        echo "  3. make -C app"
        echo ""
        echo "Build:  make -C app"
        echo "Flash:  make -C app flash"
      '';
    };
  };
}