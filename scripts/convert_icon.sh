#!/bin/bash

# Проверяем наличие ImageMagick
if ! command -v convert &> /dev/null; then
    echo "ImageMagick не установлен. Установите его с помощью:"
    echo "sudo apt-get install imagemagick"
    exit 1
fi

# Конвертируем PNG в ICO
convert resource/icons/app.png -define icon:auto-resize=256,128,64,32,16 resource/icons/app.ico

echo "Иконка успешно сконвертирована в app.ico" 