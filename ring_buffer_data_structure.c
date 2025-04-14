#include <stdio.h>
#include <stdbool.h>

#include "ring_buffer_data_structure.h"

void setup_ring_buffer(ring_buffer_t *ring_buffer, uint8_t *buffer, uint16_t buffer_size)
{
    /*
    This function initializes the ring buffer data structure type.

    This function will initialize the values of the read and write indexes (which point to buffer array slots),
    it will asign its respective buffer and its size.

    -------
    Input:
        ring_buffer_t *ring_buffer
            A ring_buffer_t custom struct which will allocate all the information and properties associated to this data structure.
        uint8_t *buffer
            An array of integers that conforms the buffer (expecting to allocate only 8-bit values per array slot).
        uint16_t buffer_size
            The size of "uint8_t *buffer" (expecting to not exceed 65536 according to the 16-bit capacity).

    -------
    Output:
        ring_buffer->buffer
            A pointer from the struct allocating the buffer.
        ring_buffer->write_index
            A default value for the write index.
        ring_buffer->read_index
            A default value for the read index.
        ring_buffer->size
            The size of the buffer.
    */

   ring_buffer->buffer = buffer;
   ring_buffer->write_index = 0;
   ring_buffer->read_index = 0;
   ring_buffer->size = buffer_size;
}

bool ring_buffer_emptiness(ring_buffer_t *ring_buffer)
{
    /*
    This function checks if the ring buffer is empty (contains no information to read).

    -------
    Input:
        ring_buffer_t *ring_buffer
            A ring_buffer_t custom struct which will allocate all the information and properties associated to this data structure.
    -------
    Output:
        bool buffer_emptiness
            A boolean specifying true or false for emptiness condition.
        
    */

    // bool buffer_emptiness:
    return (ring_buffer->write_index == ring_buffer->read_index);
    /* This function will be used to avoid a read operation when the buffer is empty.
        If a read operation is called, the next position will be empty, and it will fullfil the requirements for non-availability (resulting in an error). */
}

bool read_ring_buffer(ring_buffer_t *ring_buffer, uint8_t *byte_read)
{
    /*
    This function reads and frees the data allocated in the ring buffer data structure.

    -------
    Input:
        ring_buffer_t *ring_buffer
            A ring_buffer_t custom struct which will allocate all the information and properties associated to this data structure.
        uint8_t *byte_read
            A pointer of a char where the read data will be stored.
    -------
    Output:
        bool
            A boolean specifying true or false if something was read from the buffer.
        uint8_t *byte_read
            A modified value of the content of this pointer.  
    */

   // Checking buffer emptiness

    uint16_t instance_read_index = ring_buffer->read_index;
    // A local copy of read index is created in case an interruption modifies its original value while this function is being executed.
    // This critical condition can vulnerate the execution if it happens after the following verification (the if statement).
   if (ring_buffer_emptiness(ring_buffer) == true)
   {
        return false;
   }

    *byte_read = ring_buffer->buffer[instance_read_index];
    if (instance_read_index == (ring_buffer->size - 1))
    {
        instance_read_index = 0;
    }
    else
    {
        instance_read_index++;
    }

    ring_buffer->read_index = instance_read_index; // read index value update
    return true;
}

bool write_ring_buffer(ring_buffer_t *ring_buffer, uint8_t *byte_to_read)
{
    /*
    This function stores (or writes) incoming data from the 8-bit buffer register to the ring buffer data structure.

    -------
    Input:
        ring_buffer_t *ring_buffer
            A ring_buffer_t custom struct which will allocate all the information and properties associated to this data structure.
        uint8_t *byte_to_read
            The address of the 8-bit buffer register that stores the incoming data (to be stored in the ring_buffer).
    -------
    Output:
        bool
            A boolean specifying true or false if something was written to the buffer (in case space and data is available).
    */

   // Storing a local copy of current index state if interrupt occurs.

    uint16_t instance_write_index = ring_buffer->write_index;

    uint16_t future_write_index = instance_write_index + 1;
    if (instance_write_index == (ring_buffer->size - 1)) // Verification to avoid index overflow
    {
        future_write_index = 0;
    }

   if (ring_buffer->read_index == future_write_index)
   {
        // The buffer is full and cannot allocate more data. Future data will be ignored until the buffer is freed.
        return false;
   }
    ring_buffer->buffer[instance_write_index] = *byte_to_read;
    
    ring_buffer->write_index = future_write_index; // write index value update
    return true;
}

// In this case, the ring buffer data structure will be used to storre only incoming data (to stack in an array every incoming message before it
// is flushed at the moment of reading the content of the memory)
