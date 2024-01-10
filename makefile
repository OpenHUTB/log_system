CC := g++
TARGET := main
OBJECT := main.cpp ./ThreadPool/ThreadPool.cpp
HEAD := -I ./ThreadPool
LIBNAME := -lpthread
LIBPATH :=

$(TARGET): $(OBJECT)
	$(CC) $^ $(HEAD) $(LIBPATH) $(LIBNAME) -o $@ -std=c++11
	@echo "编译完成"
	./$@
	make clean
	@echo "已清除可执行文件"
	make backup
	@echo "备份完成"
.PHONY: clean
clean:
	rm -v $(TARGET)

.PHONY: backup
backup:
	rm -rf ../Backup/*
	cp -r ./* ../Backup