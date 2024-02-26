void src(int num);
void sink(void);

int main() {

    // Call the source function
    src(1);

    return 0;
}

// Source function
void src(int num) {
    int a = 0;
    if(num >= 1) {
        src(0);
    } else if(num == 4) {
        a++;
        src(-1);
    } else {
        sink();
    }
}

// Sink function
void sink(void) {
    int a = 0;
}