//#include <boost/interprocess/managed_shared_memory.hpp>
//#include <boost/circular_buffer.hpp>
//#include <iostream>
//
//const int IMAGE_WIDTH = 3840;
//const int IMAGE_HEIGHT = 2160;
//
//struct Image {
//    uint16_t pixels[IMAGE_WIDTH][IMAGE_HEIGHT];
//};
//
//// Define the structure to be stored in shared memory
//struct SharedMemoryData {
//    // Define your circular buffer type
//    boost::circular_buffer<Image> ringBuffer;
//};
//
//using namespace boost::interprocess;
//int main() {
//
//
//    try {
//
//        // Create a managed shared memory segment
//        // Name
//        // Size
//        managed_shared_memory segment( open_or_create, "MySharedMemory",  65536 );
//
//
//        // Allocate space for SharedMemoryData in the shared memory segment
//        SharedMemoryData* sharedData = segment.construct<SharedMemoryData>( "MyCircularBuffer"  )(); // Name of the object in shared memory
//
//        // Initialize the circular buffer within the allocated space
//        sharedData->ringBuffer.set_capacity(10);  // Set the capacity as needed
//
//        // Access and use the circular buffer
//        Image one;
//        Image two;
//        sharedData->ringBuffer.push_back(one);
//        sharedData->ringBuffer.push_back(two);
//
//        // Display the contents of the circular buffer
//        std::cout << "Circular Buffer Contents: ";
//        for (const auto& value : sharedData->ringBuffer) {
//            std::cout << value << " ";
//        }
//        std::cout << std::endl;
//
//        // ... Perform other operations as needed ...
//
//        // Destroy the shared memory objects when they are no longer needed
//        segment.destroy<SharedMemoryData>("MyCircularBuffer");
//
//    } catch (const  interprocess_exception& ex) {
//        std::cerr << "Error: " << ex.what() << std::endl;
//    }
//
//    return 0;
//}
