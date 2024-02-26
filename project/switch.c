void src(int num);
void sink(int data);

int main() {

    // Call the source function
    src(1);

    return 0;
}

// Source function
void src(int num) {
    switch(num) {
        case 0: sink(0); break;
        case 1: sink(0); break;
        default: break;
    }
}

// Sink function
void sink(int data) {
    data = 2;
}