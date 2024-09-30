#!/bin/bash

SOURCE_FILE="*.cpp"
EXECUTABLE="prog"

if [ ! -f "$SOURCE_FILE" ]; then
  echo "Исходный файл $SOURCE_FILE не найден."
  exit 1
fi

echo "Компиляция $SOURCE_FILE..."
g++ -o "$EXECUTABLE" "$SOURCE_FILE"

if [ $? -ne 0 ]; then
  echo "Ошибка компиляции."
  exit 1
fi

echo "Запуск $EXECUTABLE..."
./"$EXECUTABLE"

if [ $? -ne 0 ]; then
  echo "Ошибка выполнения."
  exit 1
fi

