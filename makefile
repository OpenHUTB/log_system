CC := g++
TARGET := main
OBJECT := main.cpp ./ThreadPool/*.cpp ./CloudBackup/*.cpp
HEADPATH := -I./ThreadPool -I./CloudBackup 
LIBNAME := -lpthread -lstdc++fs -ljsoncpp
LIBPATH :=

$(TARGET): $(OBJECT)
	$(CC) $^ $(HEADPATH) $(LIBNAME) -o $@ -std=c++11 
	@echo "编译完成，正在执行程序"
	./$@
	make clean
	@echo "已清除可执行文件"
.PHONY: clean
clean:
	rm -v $(TARGET)

.PHONY: backup
backup:
	cp -vr ./* ../LocalRepository
	@echo "备份完成"
