# Bash tab-completion for matter_build_proj
# Sourced from matter_env.sh. Completes the first argument (the Matter App
# Example) using the matter_examples=(...) list parsed from matter_build_proj
# itself, so the completion list stays in sync with the build script.

_matter_build_proj_completions() {
  local cur prev
  cur="${COMP_WORDS[COMP_CWORD]}"

  # Only complete the first argument (the example name).
  if [[ "$COMP_CWORD" -ne 1 ]]; then
    return 0
  fi

  # Locate the real matter_build_proj on PATH.
  local build_script
  build_script="$(command -v matter_build_proj 2>/dev/null)"
  if [[ -z "$build_script" || ! -r "$build_script" ]]; then
    return 0
  fi

  # Extract the names inside the matter_examples=( ... ) block.
  local examples
  examples="$(sed -n '/^matter_examples=(/,/^)/p' "$build_script" \
    | grep -oE '"[^"]+"' | tr -d '"')"

  COMPREPLY=( $(compgen -W "${examples}" -- "${cur}") )
}

complete -F _matter_build_proj_completions matter_build_proj
