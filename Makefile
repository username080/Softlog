CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lcurl
TARGET = softlog
TEST_TARGET = run_tests

SRCS = main.c reader/reader.c regex/matcher.c webhook/sender.c utils/json.c
OBJS = $(SRCS:.c=.o)

TEST_SRCS = tests/test_runner.c regex/matcher.c utils/json.c
TEST_OBJS = $(TEST_SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $(TEST_TARGET)

install: $(TARGET) softlog.service
	mkdir -p /usr/local/bin
	cp $(TARGET) /usr/local/bin/$(TARGET)
	chmod +x /usr/local/bin/$(TARGET)
	cp softlog.service /etc/systemd/system/
	systemctl daemon-reload
	@echo "Installation complete! Start softlog by running: systemctl start softlog"

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_TARGET)
