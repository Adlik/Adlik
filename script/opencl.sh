#/bin/bash -e

SCRIPT_DIR=$(
  cd "$(dirname "$0")"
  pwd
)

EXIT_FAILURE=1

uninstall_user_mode()
{
    echo Looking for previously installed user-mode driver...

    PACKAGES=("intel-opencl"
              "intel-ocloc"
              "intel-gmmlib"
              "intel-igc-core"
              "intel-igc-opencl")

    for package in "${PACKAGES[@]}"; do
        found_package=$(dpkg-query -W -f='${binary:Package}\n' "${package}")
        if [[ $? -eq 0 ]]; then
            echo "Found installed user-mode driver, performing uninstall..."
            cmd="apt-get autoremove -y $package"
            echo "$cmd"
            eval "$cmd"
            if [[ $? -ne 0 ]]; then
                echo "ERROR: failed to uninstall existing user-mode driver." >&2
                echo "Please try again manually and run the script again." >&2
                exit $EXIT_FAILURE
            fi
        fi
    done
}

download_packages()
{
    mkdir -p "$SCRIPT_DIR/neo"
    cd "$SCRIPT_DIR/neo" || exit
    
    #download 20.35.17767 driver
    curl -L -O https://github.com/intel/compute-runtime/releases/download/20.35.17767/intel-gmmlib_20.2.4_amd64.deb
    curl -L -O https://github.com/intel/compute-runtime/releases/download/20.35.17767/intel-igc-core_1.0.4756_amd64.deb
    curl -L -O https://github.com/intel/compute-runtime/releases/download/20.35.17767/intel-igc-opencl_1.0.4756_amd64.deb
    curl -L -O https://github.com/intel/compute-runtime/releases/download/20.35.17767/intel-opencl_20.35.17767_amd64.deb
    curl -L -O https://github.com/intel/compute-runtime/releases/download/20.35.17767/intel-ocloc_20.35.17767_amd64.deb
    curl -L -O https://github.com/intel/compute-runtime/releases/download/20.35.17767/intel-level-zero-gpu_1.0.17767_amd64.deb

    #verify_checksum
    curl -L -O https://github.com/intel/compute-runtime/releases/download/20.35.17767/ww35.sum
    sha256sum -c ww35.sum
    if [[ $? -ne 0 ]]; then
        echo "ERROR: checksums do not match for the downloaded packages"
        echo "       Please verify your Internet connection and make sure you have enough disk space or fix the problem manually and try again. "
        exit $EXIT_FAILURE
    fi
}

_deploy_deb()
{
    cmd="dpkg -i $1"
    echo "$cmd"
    eval "$cmd"
}

install_user_mode()
{
    echo "Installing user mode driver..."
    
    _deploy_deb "intel*.deb"
    if [[ $? -ne 0 ]]; then
        echo "ERROR: failed to install debs $cmd error"  >&2
        echo "Make sure you have enough disk space or fix the problem manually and try again." >&2
        exit $EXIT_FAILURE
    fi

    # exit from $SCRIPT_DIR/neo folder
    cd - || exit
    # clean it up
    rm -rf "$SCRIPT_DIR/neo"
}

install_prerequisites()
{
    CMDS=("apt-get -y update"
          "apt-get -y install --no-install-recommends libnuma1 ocl-icd-libopencl1")

    for cmd in "${CMDS[@]}"; do
        echo "$cmd"
        eval "$cmd"
        if [[ $? -ne 0 ]]; then
            echo "ERROR: failed to run $cmd" >&2
            echo "Problem (or disk space)?" >&2
            echo "                sudo -E $0" >&2
            echo "2. Verify that you have enough disk space, and run the script again." >&2
            exit $EXIT_FAILURE
        fi
    done
}

echo "Intel® Graphics Compute Runtime for OpenCL™ Driver installer"
uninstall_user_mode
install_prerequisites
download_packages
install_user_mode