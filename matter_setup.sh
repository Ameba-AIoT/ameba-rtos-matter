#!/bin/bash

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <path/to/ameba-rtos-sdk>"
  exit 1
fi

PATH_TO_AMEBA_RTOS_MATTER="$PWD"
PATH_TO_AMEBA_RTOS_MATTER_SCRIPTS="$PATH_TO_AMEBA_RTOS_MATTER/tools/scripts"

cd $1
PATH_TO_AMEBA_RTOS="$PWD"

cd ../connectedhomeip
PATH_TO_CONNECTEDHOMEIP="$PWD"

MATTER_ENV_SCRIPT="$PATH_TO_AMEBA_RTOS_MATTER/matter_env.sh"

environment_scripts=(
  "$PATH_TO_CONNECTEDHOMEIP/scripts/bootstrap.sh"
  "$PATH_TO_AMEBA_RTOS/env.sh"
  "$PATH_TO_CONNECTEDHOMEIP/scripts/activate.sh"
)

ameba_wifi_config=(
  "$PATH_TO_AMEBA_RTOS/component/soc/usrcfg/amebadplus/ameba_wificfg.c"
  "$PATH_TO_AMEBA_RTOS/component/soc/usrcfg/amebalite/ameba_wificfg.c"
  "$PATH_TO_AMEBA_RTOS/component/soc/usrcfg/amebasmart/ameba_wificfg.c"
  "$PATH_TO_AMEBA_RTOS/component/soc/usrcfg/amebagreen2/ameba_wificfg.c"
)

create_matter_env_script(){
  echo "Creating matter_env.sh"

  echo '#!/bin/bash' > "$MATTER_ENV_SCRIPT"
  echo '' >> "$MATTER_ENV_SCRIPT"

  for file_path in "${environment_scripts[@]}"; do
    if [ -e "$file_path" ]; then
      echo "File $file_path exist."
      if [[ "$file_path" == *"bootstrap.sh" ]]; then
        echo "# Connectedhomeip Env Bootstrap" >> "$MATTER_ENV_SCRIPT"
        echo "if [ -d \"$PATH_TO_CONNECTEDHOMEIP/.environment\" ]; then" >> "$MATTER_ENV_SCRIPT"
        echo "  echo \"$PATH_TO_CONNECTEDHOMEIP/.environment exist, skipping bootstrap...\"" >> "$MATTER_ENV_SCRIPT"
        echo "else" >> "$MATTER_ENV_SCRIPT"
        echo "  cd \"$PATH_TO_CONNECTEDHOMEIP\"" >> "$MATTER_ENV_SCRIPT"
        echo "  git submodule sync" >> "$MATTER_ENV_SCRIPT"
        echo "  git submodule update --init --recursive" >> "$MATTER_ENV_SCRIPT"
        echo "  source \"$file_path\"" >> "$MATTER_ENV_SCRIPT"
        echo "  pip install -r \"$PATH_TO_AMEBA_RTOS/tools/requirements.txt\"" >> "$MATTER_ENV_SCRIPT"
        echo "  deactivate" >> "$MATTER_ENV_SCRIPT"
        echo "fi" >> "$MATTER_ENV_SCRIPT"
      elif [[ "$file_path" == *"activate.sh" ]]; then
        echo "# Connectedhomeip Env Activate" >> "$MATTER_ENV_SCRIPT"
        echo "cd \"$PATH_TO_CONNECTEDHOMEIP\"" >> "$MATTER_ENV_SCRIPT"
        echo "source \"$file_path\"" >> "$MATTER_ENV_SCRIPT"
      else # env.sh
        echo "# Ameba Env Activate" >> "$MATTER_ENV_SCRIPT"
        echo "cd \"$PATH_TO_AMEBA_RTOS\"" >> "$MATTER_ENV_SCRIPT"
        echo "source \"$file_path\"" >> "$MATTER_ENV_SCRIPT"
      fi
      echo '' >> "$MATTER_ENV_SCRIPT"
    else
      echo "File $file_path does not exist."
    fi
  done

  echo "# Additional Environment Configuration" >> "$MATTER_ENV_SCRIPT"
  echo "cd \"$PATH_TO_AMEBA_RTOS_MATTER\"" >> "$MATTER_ENV_SCRIPT"
  echo "echo \"$PATH_TO_AMEBA_RTOS\" > .PATH_TO_AMEBA_RTOS" >> "$MATTER_ENV_SCRIPT"
  echo "export PATH=\"$PATH_TO_AMEBA_RTOS_MATTER_SCRIPTS:\$PATH\"" >> "$MATTER_ENV_SCRIPT"
  echo "chmod u+x \"$PATH_TO_AMEBA_RTOS_MATTER_SCRIPTS\"/matter_*" >> "$MATTER_ENV_SCRIPT"
}

turn_off_wifi_fast_reconnect() {
  echo "Turning off WiFi fast reconnect"
  for file_path in "${ameba_wifi_config[@]}"; do
    if [ -e "$file_path" ]; then
      sed -i 's/^\([[:space:]]*wifi_user_config\.fast_reconnect_en[[:space:]]*=[[:space:]]*\)1;/\10;/' "$file_path"
      echo "File $file_path updated."
    else
      echo "File $file_path does not exist."
    fi
  done
}

echo "Configuring Ameba RTOS SDK"

create_matter_env_script

turn_off_wifi_fast_reconnect

echo "Creating symlink to connectedhomeip"

cd "$PATH_TO_AMEBA_RTOS"

if [ ! -d third_party ];then
    mkdir third_party
else
    rm third_party/connectedhomeip
fi

cd third_party
rm -rf connectedhomeip
ln -s ../../connectedhomeip connectedhomeip

cd "$PATH_TO_AMEBA_RTOS_MATTER"

echo "Matter setup complete"
