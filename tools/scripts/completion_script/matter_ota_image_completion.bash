# Bash tab-completion for matter_ota_image
# Sourced from matter_env.sh. The only argument is the output OTA image path,
# so complete filesystem paths.

_matter_ota_image_completions() {
  local cur
  cur="${COMP_WORDS[COMP_CWORD]}"

  COMPREPLY=( $(compgen -f -- "${cur}") )
}

complete -F _matter_ota_image_completions matter_ota_image
