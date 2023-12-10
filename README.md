platformio override for WLED build cdfc0f6b71436899edfbb03c48b0c8cd90c33158

```

[platformio]
default_envs = esp32dev

[env:esp32dev]
build_flags = ${common.build_flags_esp32}
   -D SERVERNAME="\"BadgeEffects\""
   ; -DWLED_USE_MY_CONFIG
   -D WLED_AP_SSID="\"BadgeEffects\""
   -D WLED_AP_PASS="\"12345678\""
   -D DEFAULT_LED_COUNT=256
   -D WLED_ENABLE_PIXART
   -D LEDPIN=22

```


## Redrawing led matrix

* Arduino pinned to core0
* Drawing task pinned to core1
* Double buffering done in code
* Watchdog for core0 disabled until bluetooth is refactored