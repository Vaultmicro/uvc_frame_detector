#ifndef CIRCULAR_BUFFER_HPP
#define CIRCULAR_BUFFER_HPP

#include <iostream>
#include <cstring> // For handling string operations


template<typename T>
class CircularBuffer {
private:
    T* buffer;       // Buffer to store data (char or int)
    int buffer_size; // Size of the buffer
    int head;        // Position to write new data
    int tail;        // Position to read data
    int size;        // Number of elements in the buffer

public:
    // Constructor: Initializes buffer with a specified size
    CircularBuffer(int size);

    // Destructor: Frees the allocated buffer memory
    ~CircularBuffer();

    // Check if the buffer is empty
    bool isEmpty() const;

    // Check if the buffer is full
    bool isFull() const;

    // Add multiple elements to the buffer
    void enqueue(const T* data, int data_size);

    // Remove multiple elements from the buffer
    void dequeue(T* out_data, int data_size);

    // Print the current state of the buffer (for debugging)
    void printBuffer() const;
};



// Constructor: Initializes head, tail, size, and allocates buffer with the given size
template<typename T>
CircularBuffer<T>::CircularBuffer(int size) : buffer_size(size), head(0), tail(0), size(0) {
    buffer = new T[buffer_size]; // Dynamically allocate the buffer for T type (char or int)
}

// Destructor: Frees the dynamically allocated memory
template<typename T>
CircularBuffer<T>::~CircularBuffer() {
    delete[] buffer;
}

// Check if the buffer is empty
template<typename T>
bool CircularBuffer<T>::isEmpty() const {
    return size == 0;
}

// Check if the buffer is full
template<typename T>
bool CircularBuffer<T>::isFull() const {
    return size == buffer_size;
}

// Add multiple elements to the buffer
template<typename T>
void CircularBuffer<T>::enqueue(const T* data, int data_size) {
    for (int i = 0; i < data_size; i++) {
        if (isFull()) {
            // std::cout << "Buffer is full. Overwriting oldest data.\n";
            // Move tail to overwrite oldest data
            tail = (tail + 1) % buffer_size;
            size--;
        }
        // Add data at head position and move head forward
        buffer[head] = data[i];
        head = (head + 1) % buffer_size;
        size++;
    }
}

// Remove multiple elements from the buffer
template<typename T>
void CircularBuffer<T>::dequeue(T* out_data, int data_size) {
    if (data_size > size) {
        throw std::runtime_error("Requested dequeue size exceeds buffer size.");
    }

    for (int i = 0; i < data_size; i++) {
        if (isEmpty()) {
            throw std::runtime_error("Buffer is empty.");
        }

        // Read data from tail position and move tail forward
        out_data[i] = buffer[tail];
        tail = (tail + 1) % buffer_size;
        size--;
    }
}

// Print the current state of the buffer (for debugging)
template<typename T>
void CircularBuffer<T>::printBuffer() const {
    std::cout << "Buffer: ";
    for (int i = 0; i < buffer_size; i++) {
        std::cout << buffer[i] << " ";
    }
    std::cout << "\nHead: " << head << ", Tail: " << tail << ", Size: " << size << "\n";
}


#endif // CIRCULAR_BUFFER_HPP
