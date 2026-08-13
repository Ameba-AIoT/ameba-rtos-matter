# Bash tab-completion for matter_apply_conf
# Sourced from matter_env.sh. Completes the first argument (the Matter device
# config) using the matter_device_configs=(...) list parsed from
# matter_apply_conf itself, so the completion list stays in sync with the script.

_matter_apply_conf_completions() {
  local cur
  cur="${COMP_WORDS[COMP_CWORD]}"

  # Only complete the first argument (the device config name).
  if [[ "$COMP_CWORD" -ne 1 ]]; then
    return 0
  fi

  # Locate the real matter_apply_conf on PATH.
  local conf_script
  conf_script="$(command -v matter_apply_conf 2>/dev/null)"
  if [[ -z "$conf_script" || ! -r "$conf_script" ]]; then
    return 0
  fi

  # Extract the names inside the matter_device_configs=( ... ) block.
  local configs
  configs="$(sed -n '/^matter_device_configs=(/,/^)/p' "$conf_script" \
    | grep -oE '"[^"]+"' | tr -d '"')"

  COMPREPLY=( $(compgen -W "${configs}" -- "${cur}") )
}

complete -F _matter_apply_conf_completions matter_apply_conf
