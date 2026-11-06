name: ruby-hito2

on:
  workflow_dispatch:

jobs:
  ruby:
    runs-on: ubuntu-latest
    container: devkitpro/devkita64:latest
    steps:
      - uses: actions/checkout@v4

      - name: Herramientas del PC
        run: |
          apt-get update
          apt-get install -y build-essential curl ca-certificates autoconf bison \
            zlib1g-dev libyaml-dev libffi-dev libssl-dev

      - name: Librerias de Switch
        run: |
          PM=$(command -v dkp-pacman || command -v pacman)
          $PM -Syu --noconfirm
          $PM -S --noconfirm switch-zlib

      - name: Compilar Ruby
        run: bash scripts/build-ruby.sh

      - name: Subir logs
        if: always()
        uses: actions/upload-artifact@v4
        with:
          name: ruby-logs
          path: |
            build/*.log
            build/cross/config.log
          if-no-files-found: warn
