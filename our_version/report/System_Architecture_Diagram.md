# Minh họa Kiến trúc Hệ thống Zombie Bird

Để minh họa một cách trực quan đoạn mô tả kiến trúc phần mềm mà bạn vừa cung cấp, phương pháp tốt nhất là sử dụng biểu đồ cấu trúc (Architecture Diagram) chia làm hai tầng rõ rệt. Dưới đây là 2 cách tiếp cận sử dụng **Mermaid.js** (một công cụ vẽ biểu đồ bằng text được hỗ trợ sẵn trên Markdown và GitHub) để bạn có thể chèn trực tiếp vào báo cáo của mình.

### Lựa chọn 1: Biểu đồ Luồng (Flowchart) - *Khuyên dùng cho Báo cáo Tổng quan*
Biểu đồ này rất xuất sắc trong việc thể hiện "luồng" dữ liệu và cấu trúc tầng (Layer) của toàn bộ hệ thống. Nó giúp người đọc dễ hình dung từ Vòng lặp chính (Main Loop) đi xuống tới tận các dữ liệu vật lý cơ bản.

```mermaid
graph TD
    %% Định nghĩa màu sắc
    classDef layer1 fill:#f9f2f4,stroke:#c0392b,stroke-width:2px;
    classDef layer2 fill:#eaf2f8,stroke:#2980b9,stroke-width:2px;
    classDef entity fill:#fef9e7,stroke:#f39c12,stroke-width:2px;
    classDef system fill:#e8f8f5,stroke:#27ae60,stroke-width:2px;

    %% TẦNG TRÊN: QUẢN LÝ LUỒNG
    subgraph Layer1 [Tầng 1: Quản Lý Luồng Ứng Dụng]
        MainLoop((Vòng lặp chính<br/>Main Loop))
        
        subgraph StateManagerStack [StateManager: Ngăn xếp]
            direction TB
            StateTop["IState (Đang hoạt động)<br>GameplayState<hr>+ onEnter()<br>+ update()<br>+ draw()<br>+ handleEvent()"]
            StateMid["IState (Tạm dừng)<br>LevelSelectState"]
            StateTop -.- StateMid
        end
        
        MainLoop ==>|Gọi hàm<br/>(Không cần biết lớp cụ thể)| StateTop
    end
    class Layer1 layer1;

    %% TẦNG DƯỚI: THỰC THI GAMEPLAY
    subgraph Layer2 [Tầng 2: Thực Thi Gameplay]
        direction TB
        
        EntityHierarchy{"Phân cấp Vật thể<br/>(Entity: Bird, Pig, Block)"}
        
        subgraph Components [Các thành phần]
            Visual["Tài nguyên Hình ảnh<br/>(SFML Sprite)"]
            PhysicsData["Dữ liệu Vật lý<br/>(PhysicsBody)"]
        end
        
        subgraph CoreSystems [Hệ thống Xử lý Trung tâm]
            PhysicsWorld["PhysicsWorld<br/>(Tích hợp chuyển động & Lực)"]
            CollisionSys["CollisionSystem<br/>(Phát hiện & Giải quyết va chạm)"]
        end
        
        EntityHierarchy ==>|Liên kết hiển thị| Visual
        EntityHierarchy ==>|Liên kết vật lý| PhysicsData
        
        PhysicsWorld <==>|Đọc/Ghi dữ liệu| PhysicsData
        CollisionSys <==>|Kiểm tra tọa độ/hình khối| PhysicsData
        PhysicsWorld -.->|Phụ thuộc/Sử dụng| CollisionSys
    end
    class Layer2 layer2;
    class EntityHierarchy entity;
    class PhysicsWorld,CollisionSys system;

    %% Kết nối 2 tầng
    StateTop ==>|Quản lý & Chạy| EntityHierarchy
```

---

### Lựa chọn 2: Biểu đồ Lớp (Class Diagram) - *Khuyên dùng cho Báo cáo Kỹ thuật (Technical)*
Nếu báo cáo của bạn hướng nhiều đến việc thiết kế OOP (Object-Oriented Programming) và cần thể hiện quan hệ kế thừa (Inheritance) hay đa hình (Polymorphism) rõ rệt, đây là biểu đồ phù hợp nhất.

```mermaid
classDiagram
    direction TB
    
    namespace Tầng_Quản_Lý_Luồng_Ứng_Dụng {
        class Vòng_Lặp_Chính
        class StateManager {
            -stack~IState~ states
            +pushState(IState)
            +popState()
            +changeState(IState)
        }
        class IState {
            <<interface>>
            +onEnter()
            +update()
            +draw()
            +handleEvent()
        }
        class GameplayState
        class LevelSelectState
    }

    Vòng_Lặp_Chính --> StateManager : Gọi update(), draw()
    StateManager *-- IState : Duy trì ngăn xếp (Stack)
    IState <|-- GameplayState : Kế thừa (Đa hình)
    IState <|-- LevelSelectState : Kế thừa (Đa hình)

    namespace Tầng_Thực_Thi_Gameplay {
        class Entity {
            +update()
            +draw()
        }
        class PhysicsWorld {
            +step(dt)
            -Áp dụng lực & Trọng lực
            -Tích hợp chuyển động
        }
        class CollisionSystem {
            <<static>>
            +detectCollision(bodyA, bodyB)
            -Giải quyết va chạm
        }
        class Sprite_HìnhẢnh {
            (Giao diện Đồ họa SFML)
        }
        class PhysicsBody_DữLiệu {
            (Vị trí, Vận tốc, Quán tính)
        }
    }

    GameplayState *-- Entity : Quản lý vòng đời
    Entity *-- Sprite_HìnhẢnh : Liên kết hiển thị
    Entity *-- PhysicsBody_DữLiệu : Liên kết vật lý
    
    PhysicsWorld --> PhysicsBody_DữLiệu : Tác động vật lý
    CollisionSystem --> PhysicsBody_DữLiệu : Đo đạc va chạm
    PhysicsWorld ..> CollisionSystem : Sử dụng
```

### Cách sử dụng file này:
Bạn chỉ cần sao chép toàn bộ phần code nằm trong cặp dấu 3 nháy (```mermaid ... ```) và dán trực tiếp vào file báo cáo Markdown của bạn trên Github hoặc Notion. Nó sẽ tự động biến thành một biểu đồ chuyên nghiệp. 

*Gợi ý: Với nội dung bạn mô tả, **Lựa chọn 1 (Flowchart)** bám sát từng từ trong câu văn của bạn một cách tốt nhất.*
