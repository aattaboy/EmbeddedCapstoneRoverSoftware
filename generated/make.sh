for file in `ls | grep .pbo$`; do
  ~/embedded2016/protobono/proboc_pic32 < $file  
done
clang-format -i *.h *.c
