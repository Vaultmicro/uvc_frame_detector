#include "other_file.hpp"
#include <string>
#include <chrono>
#include <thread>
#include <random>

std::string generate_random_text() {
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(1, 100);
    return "Random value: " + std::to_string(dist(rng));
}

std::string generate_random_string() {
    static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, sizeof(charset) - 2); // 마지막 '\0' 제외

    std::string random_string;
    for (int i = 0; i < 10; ++i) {
        random_string += charset[dist(rng)];
    }
    return random_string;
}


void printing_thread_func_1(WindowData* data) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(data->mutex);
        if (data->stop_flag) break;
        data->custom_text = generate_random_text();
    }
}

void printing_thread_func_2(WindowData* data) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(data->mutex);
        if (data->stop_flag) break;
        data->custom_text = generate_random_text();
    }
}

void printing_thread_func_3(WindowData* data) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(data->mutex);
        if (data->stop_flag) break;
        data->custom_text = generate_random_text();
    }
}

void printing_thread_func_4(WindowData* data) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(data->mutex);
        if (data->stop_flag) break;
        data->custom_text = generate_random_text();
    }
}

void printing_thread_func_5(WindowData* data) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(data->mutex);
        if (data->stop_flag) break;
        data->custom_text = generate_random_text();
    }
}

void printing_thread_func_6(WindowData* data) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(data->mutex);
        if (data->stop_flag) break;
        data->custom_text = generate_random_text();
    }
}

void one_thread_func(WindowManager* manager) {
    int window_index = 0;

    while (true) {
        WindowData& data = manager->getWindowData(window_index);
        {
            std::lock_guard<std::mutex> lock(data.mutex);

            if (data.stop_flag) return;

            data.custom_text = generate_random_string();
        }

        window_index = (window_index + 1) % manager->getWindowCount();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
