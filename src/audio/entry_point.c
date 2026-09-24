#include <pico/stdlib.h>
#include <stdio.h>

// This will be run in a different core than the display processing
void audio_processing_thread_entry_point()
{
    while (true)
    {
        printf("Audio processing happens here...\n");
        sleep_ms(1000);
    }
    
}