#!/bin/sh

cd ..
find src -iname "*.*pp" | xargs xgettext -o translate/messages.pot --c++ --add-comments=/ --keyword=_ --keyword=C_:1c,2 
