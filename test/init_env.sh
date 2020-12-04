_shtool_export_echo_usage(){
    echo "Export variables to ${1}"
    echo "Usage: ${2} var1 [var2 ...]"
}

# export variables to PATH
shtool_export_path(){
    if [ $# -lt 1 ]
    then 
        _shtool_export_echo_usage 'PATH' 'shtool_export_path'
        return 1
    fi
    for i in "$@"
    do
        PATH="${i}${PATH:+:${PATH}}"
    done
    export PATH
}

shtool_export_pypath(){
    if [ $# -lt 1 ]
    then 
        _shtool_export_echo_usage 'PYTHONPATH' 'shtool_export_pypath'
        return 1
    fi
    for i in "$@"
    do 
        PYTHONPATH="${i}${PYTHONPATH:+:${PYTHONPATH}}"
    done
    export PYTHONPATH
}

# export variables to LIBRARY_PATH and LD_LIBRARY_PATH
shtool_export_lib(){
    if [ $# -lt 1 ]
    then 
        _shtool_export_echo_usage 'LIBRARY_PATH and LD_LIBRARY_PATH' 'shtool_export_lib'
        return 1
    fi
    for i in "$@"
    do
        LIBRARY_PATH="${i}${LIBRARY_PATH:+:${LIBRARY_PATH}}"
        LD_LIBRARY_PATH="${i}${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
    done
    export LIBRARY_PATH
    export LD_LIBRARY_PATH
}

# export variables to INCLUDE_PATH and CPLUS_INCLUDE_PATH
shtool_export_inc(){
    if [ $# -lt 1 ]
    then 
        _shtool_export_echo_usage 'INCLUDE_PATH and CPLUS_INCLUDE_PATH' 'shtool_export_inc'
        return 1
    fi
    for i in "$@"
    do
        INCLUDE_PATH="${i}${INCLUDE_PATH:+:${INCLUDE_PATH}}"
        CPLUS_INCLUDE_PATH="${i}${CPLUS_INCLUDE_PATH:+:${CPLUS_INCLUDE_PATH}}"
    done
    export INCLUDE_PATH
    export CPLUS_INCLUDE_PATH
}

# export variables to CPLUS_INCLUDE_PATH
shtool_export_cplus_inc(){
    if [ $# -lt 1 ]
    then 
        _shtool_export_echo_usage 'CPLUS_INCLUDE_PATH' 'shtool_export_cplus_inc'
        return 1
    fi
    for i in "$@"
    do
        CPLUS_INCLUDE_PATH="${i}${CPLUS_INCLUDE_PATH:+:${CPLUS_INCLUDE_PATH}}"
    done
    export CPLUS_INCLUDE_PATH
}


shtool_export_lib ${PWD}/../lib
shtool_export_inc ${PWD}/../include