# Các Công thức Toán học & Vật lý trong Zombie Bird

Tài liệu này trình bày chi tiết các công thức toán học và thuật toán vật lý cốt lõi được sử dụng trong engine vật lý tùy chỉnh của dự án Zombie Bird.

---

## 1. Tích phân Euler Bán ngầm (Di chuyển)
**Vị trí:** `PhysicsWorld::step`

### Các Công thức:
1. **Cập nhật Vận tốc:** $v_{new} = v_{old} + (Gravity \cdot scale + Acceleration) \cdot \Delta t$
2. **Cập nhật Vận tốc Góc:** $\omega_{new} = \omega_{old} + AngularAcceleration \cdot \Delta t$
3. **Cập nhật Vị trí:** $p_{new} = p_{old} + v_{new} \cdot \Delta t$
4. **Cập nhật Góc xoay:** $\theta_{new} = \theta_{old} + \omega_{new} \cdot \Delta t$

### Cách hoạt động:
Đây là thuật toán di chuyển các vật thể trong thế giới. Đầu tiên nó tính toán vận tốc *mới* dựa trên các lực (như trọng lực), và sau đó sử dụng vận tốc *mới* đó để cập nhật vị trí.
> **Giải thích đơn giản:** Tưởng tượng bạn đang lái ô tô. Đầu tiên, bạn đạp chân ga để tăng tốc độ (cập nhật vận tốc). Sau đó, tốc độ mới nhanh hơn của bạn sẽ quyết định bạn đi được bao xa trên đường trong giây tiếp theo (cập nhật vị trí).

---

## 2. Phép xoay Vector 2D (Chuyển đổi Tọa độ)
**Vị trí:** `CollisionSystem::rotateVec`

### Công thức:
Cho một vector $(x, y)$ và một góc $\theta$:
*   $x_{new} = x \cdot \cos(\theta) - y \cdot \sin(\theta)$
*   $y_{new} = x \cdot \sin(\theta) + y \cdot \cos(\theta)$

### Cách hoạt động:
Công thức này xoay một điểm quanh gốc tọa độ $(0,0)$ một góc nhất định. Trong engine, nó được dùng để chuyển đổi tọa độ từ "Không gian Thế giới" (World Space - toàn bộ màn hình) sang "Không gian Cục bộ" (Local Space - tương đối so với một hộp bị xoay cụ thể).
> **Giải thích đơn giản:** Nếu bạn nghiêng đầu 45 độ, cả thế giới trông như bị nghiêng. Công thức toán học này "làm nghiêng thế giới" tạm thời để engine có thể dễ dàng đo khoảng cách đến một chiếc hộp bị nghiêng giống như thể nó đang đứng thẳng.

---

## 3. Va chạm Hình tròn vs. Hình tròn (Bình phương Khoảng cách)
**Vị trí:** `CollisionSystem::circleVsCircle`

### Công thức:
*   $Distance^2 = (x_2 - x_1)^2 + (y_2 - y_1)^2$
*   **Điều kiện Va chạm:** $Distance^2 < (Radius_1 + Radius_2)^2$

### Cách hoạt động:
Để kiểm tra xem hai hình tròn có chồng lên nhau không, engine tính khoảng cách giữa hai tâm của chúng. Thay vì dùng phép tính Căn bậc hai của định lý Pytago (rất chậm đối với máy tính), nó đơn giản là so sánh *bình phương* khoảng cách với *bình phương* tổng của hai bán kính.
> **Giải thích đơn giản:** Nếu khoảng cách giữa tâm của hai chú chim nhỏ hơn tổng kích thước của chúng, chúng đã đâm vào nhau. Việc bỏ qua phép tính căn bậc hai giúp game chạy nhanh hơn rất nhiều khi phải kiểm tra hàng trăm vật thể.

---

## 4. Định lý Trục tách rời (SAT) cho OBBs
**Vị trí:** `CollisionSystem::obbVsOBB`

### Thuật toán:
1. Xác định 4 trục cục bộ (vector pháp tuyến) của hai Hộp giới hạn định hướng (OBB).
2. Đối với mỗi trục, chiếu các góc của cả hai hộp lên trục đó sử dụng **Tích vô hướng** ($A \cdot B = A_x B_x + A_y B_y$).
3. Tìm hình chiếu nhỏ nhất và lớn nhất ("bóng đổ") của mỗi hộp trên trục đó.
4. **Điều kiện:** Nếu có *bất kỳ* trục nào mà các bóng đổ KHÔNG chồng lên nhau, thì hai hộp không va chạm.
5. Nếu tất cả các bóng đổ đều chồng lên nhau, chúng đang va chạm. Trục có độ chồng lấn nhỏ nhất chính là pháp tuyến va chạm (collision normal).

### Cách hoạt động:
SAT là một phương pháp toán học để chứng minh hai hình lồi không chạm nhau.
> **Giải thích đơn giản:** Hãy tưởng tượng chiếu đèn pin vào hai chiếc hộp từ 4 góc vuông góc khác nhau. Nếu bạn nhìn vào bóng của chúng trên tường, và bạn có thể thấy một khe hở giữa các bóng từ *dù chỉ một góc chiếu*, thì hai chiếc hộp đó không chạm nhau.

---

## 5. Bộ giải Xung lực Tuần tự (Giải quyết Va chạm)
**Vị trí:** `PhysicsWorld::step` (Iterative Solver)

### Các Công thức:
1. **Vận tốc tương đối ($V_{rel}$):** Tốc độ mà hai điểm tiếp xúc đang đâm vào nhau.
   $V_{rel} = (V_B + \omega_B \times r_B) - (V_A + \omega_A \times r_A)$
2. **Khối lượng hiệu dụng ($K$):** Cảm giác độ nặng của các vật thể tại điểm tiếp xúc cụ thể đó, xét đến khối lượng và mức độ dễ quay của chúng (quán tính).
   $K_{normal} = \frac{1}{Mass_A} + \frac{1}{Mass_B} + \frac{(r_A \times normal)^2}{Inertia_A} + \frac{(r_B \times normal)^2}{Inertia_B}$
3. **Xung lực ($j$):** Lực tức thời được áp dụng để đẩy chúng ra xa nhau, có tính đến độ đàn hồi ($e$).
   $j = \frac{-(1 + e) \cdot V_{rel}}{K_{normal}}$

### Cách hoạt động:
Khi các vật thể va chạm, engine không chỉ "dịch chuyển tức thời" chúng ra xa nhau. Nó tính toán chính xác lượng động lượng đột ngột (Xung lực) cần thiết để ngăn chúng di chuyển xuyên qua nhau, và áp dụng nó lên cả hai vật thể theo các hướng ngược nhau.
> **Giải thích đơn giản:** Hãy nghĩ về hai quả bida va chạm. Bộ giải tính toán chính xác lực đập của chúng dựa trên trọng lượng và tốc độ, sau đó ngay lập tức truyền năng lượng đó để chúng nảy ra xa nhau một cách chân thực.

---

## 6. Ổn định Baumgarte (Sửa lỗi Chồng lấn)
**Vị trí:** `PhysicsWorld::step` (Pre-step Bias)

### Công thức:
$BiasVelocity = \frac{-\beta}{\Delta t} \cdot \max(0, Penetration - Slop)$

### Cách hoạt động:
Vì vật lý được tính toán theo từng bước rời rạc (khung hình), các vật thể thường lún nhẹ vào nhau trước khi engine phát hiện ra. Sự ổn định Baumgarte thêm vào một "vận tốc ảo" nhỏ để nhẹ nhàng đẩy các vật thể ra xa nhau trong suốt vài khung hình mà không thêm năng lượng nảy dư thừa vào hệ thống.
> **Giải thích đơn giản:** Nếu một khối gỗ nặng lún nhẹ vào mặt đất giữa các khung hình, công thức này hoạt động giống như một chiếc lò xo nhẹ nhàng, từ từ đẩy khối gỗ nổi trở lại mặt đất để nó không bị kẹt vĩnh viễn bên trong sàn nhà.
