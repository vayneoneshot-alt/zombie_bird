# Chi tiết triển khai - Kiến trúc sư trưởng & Phát triển Core Engine (Giai đoạn 1 & 2)

Dựa trên việc xem xét mã nguồn (`IState.h`, `StateManager.h`, `StateManager.cpp`, `PhysicsWorld.h`, `PhysicsWorld.cpp`), dưới đây là phân tích chi tiết về mọi thứ bạn đã triển khai với vai trò **Kiến trúc sư trưởng & Phát triển Core Engine** trong Giai đoạn 1 và 2:

### 1. Tính trừu tượng: Giao diện `IState` và `StateManager`
Bạn đã thiết kế một hệ thống quản lý trạng thái rất linh hoạt, tách biệt hoàn toàn logic quản lý luồng ứng dụng khỏi nội dung từng màn chơi cụ thể.

*   **[`IState` Interface](file:///c:/hoctap/C++/zombie_bird/our_version/src/states/IState.h#L6-L15):** Một lớp thuần ảo (pure virtual class) định nghĩa các phương thức cơ bản như `onEnter()`, `update()`, `draw()`, v.v.
    *   *Giải thích đơn giản:* Nó giống như một "bản hợp đồng". Bất kỳ màn hình nào trong game (Menu, Đang chơi, Thắng, Thua) đều phải ký bản hợp đồng này, cam kết rằng chúng có khả năng tự cập nhật hình ảnh và tự vẽ lên màn hình.
    *   *Phân tích chi tiết:* Việc thiết kế `IState` áp dụng hoàn hảo tính Trừu tượng (Abstraction) và Đa hình (Polymorphism) của OOP. Trò chơi gốc (main loop) không cần biết nó đang chạy `MainMenuState` hay `GameplayState`, nó chỉ gọi `update()` trên `IState`. Điều này giúp code cực kỳ dễ mở rộng khi muốn thêm màn hình mới.
*   **[`StateManager`](file:///c:/hoctap/C++/zombie_bird/our_version/src/states/StateManager.h#L8-L20):** Một ngăn xếp (stack) chứa các `IState` (thông qua `std::unique_ptr`). Nó xử lý các thao tác chuyển cảnh như `pushState`, `popState`, `changeState`.
    *   *Giải thích đơn giản:* Nó giống như một quyển sách có thể lật trang qua lại. Ngăn xếp (stack) cho phép game "nhớ" trạng thái trước đó. Ví dụ: bạn đang ở màn chơi (Gameplay), mở menu tạm dừng (push PauseState lên trên), sau đó đóng tạm dừng (pop PauseState) và quay lại đúng màn chơi cũ mà không cần nạp lại từ đầu.
    *   *Phân tích chi tiết:* Cấu trúc này rất an toàn về mặt bộ nhớ nhờ sử dụng `std::unique_ptr`, đảm bảo không có memory leak khi hủy một trạng thái. Nó cũng tạo ra sự tách biệt (decoupling) tuyệt vời giữa các màn chơi.

### 2. Tích hợp Core `PhysicsWorld`
Bạn đã xây dựng một bộ giải quyết vật lý (physics solver) tùy chỉnh mạnh mẽ thay vì dùng thư viện có sẵn, tích hợp vòng lặp cập nhật vật lý một cách đồng bộ.

*   **[Vòng lặp Tích phân (`step`)](file:///c:/hoctap/C++/zombie_bird/our_version/src/core/PhysicsWorld.cpp#L20-L204):** Trong `PhysicsWorld.cpp`, bạn đã thiết kế một vòng lặp đa bước phức tạp để giải quyết va chạm và cập nhật vị trí.
    *   *Giải thích đơn giản:* Bộ máy vật lý của bạn hoạt động như một cỗ máy thời gian chạy từng khung hình một. Đầu tiên, nó cộng thêm trọng lực và lực kéo. Sau đó, nó kiểm tra xem có vật nào đâm vào nhau không (bằng cách gọi công cụ của hệ thống va chạm). Nếu có, nó giải quyết va chạm (đẩy chúng ra xa hoặc làm nảy lên bằng lực ma sát và đàn hồi). Cuối cùng, nó dời tất cả vật thể đến vị trí mới.
    *   *Phân tích chi tiết:* Bạn đã triển khai thuật toán giải quyết va chạm lặp (Iterative Solver) với khởi động ấm (Warm Starting). Việc lưu giữ `normalImpulse` và `tangentImpulse` giữa các khung hình (frames) giúp các vật thể xếp chồng lên nhau (như khối gỗ) ổn định, không bị rung lắc (jitter). Đây là một kỹ thuật vật lý nâng cao rất ấn tượng đối với một custom engine.
*   **Đóng gói Logic Kết xuất (Render Pipeline):** Mọi bản vẽ đồ họa đều được đưa qua hàm `draw()` của hệ thống trạng thái, gắn kết chặt chẽ vòng lặp trò chơi của SFML với các đối tượng thực thể (Entities).
    *   *Giải thích đơn giản:* Bạn đã làm người "nhạc trưởng" điều phối toàn bộ đồ họa. Game sẽ luôn chạy logic toán học và vật lý trước, sau đó mới vẽ mọi thứ ra màn hình một cách mượt mà và đúng thứ tự.
    *   *Phân tích chi tiết:* Việc đóng gói SFML `RenderWindow` vào trong cơ chế truyền tải (passing it by reference qua hàm `draw()`) giúp game không bị phụ thuộc cứng vào biến toàn cục (global variables). Nó là một kiến trúc sạch (clean architecture), hỗ trợ rất tốt nếu muốn thay thế engine đồ họa sau này.
