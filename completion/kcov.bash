# kcov(1) completion                                       -*- shell-script -*-

_kcov()
{
    local cur prev words cword split
    _init_completion -s || return

    case "$prev" in
        --pid|-p)
            _pids
            return
            ;;
        --sort-type|-s)
            COMPREPLY=( $( compgen -W 'filename percentage lines uncovered' \
                -- "$cur" ) )
            return
            ;;
        --include-path|--exclude-path)
            _filedir
            return
            ;;
        --replace-src-path) # FIXME
            _filedir
            return
            ;;
        --limits|-l)
            split=false
            if [[ "$cur" == ?*,* ]]; then
                prev="${cur%,*}"
                cur="${cur##*,}"
                split=true
            fi
            COMPREPLY=( $( compgen -W "{0..100}" -- "$cur" ) )
            if $split; then
                COMPREPLY=( ${COMPREPLY[@]/#/"$prev,"} )
            else
                compopt -o nospace
            fi
            return
            ;;
        --title|-t|--include-pattern|--exclude-pattern|--path-strip-level)
            # argument required but no completions available
            return
            ;;
    esac

    $split && return

    if [[ "$cur" == -* ]]; then
        COMPREPLY=( $( compgen -W '$( _parse_help "$1" --help )' -- "$cur" ) )
        [[ $COMPREPLY == *= ]] && compopt -o nospace
        return
    fi

    _filedir
} && complete -F _kcov kcov

# ex: ts=4 sw=4 et filetype=sh
