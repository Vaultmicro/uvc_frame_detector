#ifndef CIRCULAR_BUFFER_HPP
#define CIRCULAR_BUFFER_HPP

#include <iostream>

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

#endif // CIRCULAR_BUFFER_HPP
