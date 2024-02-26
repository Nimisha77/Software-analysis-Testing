void src(void);
void sink(int data);

int main() {

    // Call the source function
    src();

    return 0;
}

// Source function
void src(void) {
    int data = 42;

    // Call the sink function with the data
    // sink(data);
}

// Sink function
void sink(int data) {
    data = 2;
}