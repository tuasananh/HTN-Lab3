# Báo cáo thực hành 3 – Đo công suất của STM32F429 ở các chế độ hoạt động

Điền kết quả thực nghiệm vào các ô `…`. Trong các bảng đo, U và I lần lượt là điện áp và dòng điện đọc trên đồng hồ USB. Ghi công suất hiển thị vào cột **P đo**. Nếu đồng hồ chỉ hiển thị điện áp và dòng điện, tính **P đo (mW) = U (V) × I (mA)**. Cột **P hiệu chỉnh** bằng P đo trừ công suất không tải ở mục 3.0. [Đề thực hành](Lab3_IT4210E-Low%20power%20modes.pdf)

## 3.0. Đo công suất không tải của đồng hồ USB

Ngày 27/11/2026, đồng hồ USB được cắm vào máy tính và đọc số đo khi đầu ra còn trống. Sau đó, bo STM32F429 Discovery được nối vào đồng hồ để đo lần thứ hai.

| Điều kiện đo | U (V) | I (mA) | P tính từ U × I (mW) |
| --- | ---: | ---: | ---: |
| Đầu ra đồng hồ để trống | 5 | 0 | 0 |
| Đã nối bo STM32F429 Discovery | 5 | 80 | 400 |

Công suất không tải dùng để hiệu chỉnh các phép đo sau là **0 mW**. Giá trị **400 mW** là số đo kiểm tra ban đầu khi đã nối bo. Chương trình và chế độ hoạt động của bo tại thời điểm đó chưa được ghi lại.

## 3.1. Bài 1 – Ảnh hưởng của tần số HCLK đến công suất ở chế độ Run

Đề ghi cùng một trạng thái **LD3 sáng, LD4 tắt** trong bốn khoảng liên tiếp, mỗi khoảng 5 giây. Mỗi hàng dưới đây ứng với một lần đọc. Ghi thời gian và điện năng tích lũy theo bộ đếm trên đồng hồ tại thời điểm đọc. Nếu đồng hồ hiển thị Wh, nhân với 1.000 để đổi sang mWh.

| HCLK (MHz) | Khoảng 5 giây | U (V) | I (mA) | P đo (mW) | P hiệu chỉnh (mW) | Thời gian trên đồng hồ (giờ:phút:giây) | Điện năng tích lũy (mWh) |
| ---: | ---: | ---: | ---: | ---: | ---: | --- | ---: |
| 16 | 1 | … | … | … | … | … | … |
| 16 | 2 | … | … | … | … | … | … |
| 16 | 3 | … | … | … | … | … | … |
| 16 | 4 | … | … | … | … | … | … |
| 96 | 1 | … | … | … | … | … | … |
| 96 | 2 | … | … | … | … | … | … |
| 96 | 3 | … | … | … | … | … | … |
| 96 | 4 | … | … | … | … | … | … |
| 180 | 1 | … | … | … | … | … | … |
| 180 | 2 | … | … | … | … | … | … |
| 180 | 3 | … | … | … | … | … | … |
| 180 | 4 | … | … | … | … | … | … |

| Nội dung cần báo cáo | Kết quả hoặc nhận xét |
| --- | --- |
| Quan sát LD3 và LD4 khi chạy chương trình trên bo. | … |
| Với cùng trạng thái LED, công suất hiệu chỉnh thay đổi thế nào khi HCLK tăng từ 16 lên 96 rồi 180 MHz? Dẫn số đo để so sánh. | … |
| Ngoài CPU, những thành phần nào góp phần vào công suất đo tại cổng USB? | … |

## 3.2. Bài 2 – Đo công suất ở chế độ Sleep

Cả hai phép thử dùng HCLK 96 MHz. Ở mục 3.2.1, ngắt SysTick đánh thức CPU trong thời gian chờ giữa hai lần đổi trạng thái LED. Ở mục 3.2.2, SysTick được tạm dừng và nút USER (PA0) đánh thức CPU bằng ngắt ngoài EXTI0.

| Phép thử | Trạng thái LD3 và LD4 | U (V) | I (mA) | P đo (mW) | P hiệu chỉnh (mW) |
| --- | --- | ---: | ---: | ---: | ---: |
| 3.2.1 – SysTick | Cả hai sáng | … | … | … | … |
| 3.2.1 – SysTick | Cả hai tắt | … | … | … | … |
| 3.2.2 – Nút PA0 | Cả hai sáng | … | … | … | … |
| 3.2.2 – Nút PA0 | Cả hai tắt | … | … | … | … |

| Nội dung cần báo cáo | Kết quả hoặc nhận xét |
| --- | --- |
| 3.2.1 – Khoảng thời gian quan sát được giữa hai lần đổi trạng thái LED có khớp với 5 giây không? | … |
| 3.2.2 – Hai LED thay đổi thế nào sau mỗi lần nhấn PA0? | … |
| So với chế độ Run ở 96 MHz trong bài 1, công suất ở mỗi phép thử Sleep thay đổi thế nào? Giải thích dựa trên thời gian CPU ngủ, nguồn đánh thức và trạng thái LED khi đo. | … |

## 3.3. Bài 3 – Đo công suất ở chế độ Stop

HCLK trong bảng là tần số cấu hình khi CPU chạy. Dùng nút PA0 để đảo trạng thái cả hai LED, sau đó đọc số đo khi CPU đã trở lại Stop.

| HCLK trước khi vào Stop (MHz) | Trạng thái LD3 và LD4 | U (V) | I (mA) | P đo (mW) | P hiệu chỉnh (mW) |
| ---: | --- | ---: | ---: | ---: | ---: |
| 16 | Cả hai sáng | … | … | … | … |
| 16 | Cả hai tắt | … | … | … | … |
| 96 | Cả hai sáng | … | … | … | … |
| 96 | Cả hai tắt | … | … | … | … |
| 180 | Cả hai sáng | … | … | … | … |
| 180 | Cả hai tắt | … | … | … | … |

| Nội dung cần báo cáo | Kết quả hoặc nhận xét |
| --- | --- |
| Khi nhấn PA0 để đánh thức CPU, LD3 và LD4 thay đổi thế nào? | … |
| Ở cùng cấu hình 96 MHz và cùng trạng thái LED, Stop có tiêu thụ ít công suất hơn Sleep ở bài 2 không? Dẫn số đo và giải thích. | … |

## 3.4. Bài 4 – Đo công suất ở chế độ Standby

Đo hai trạng thái LED trong khoảng 5 giây CPU chạy sau khi cấp nguồn hoặc thức dậy. Đo hàng Standby sau khi CPU đã vào chế độ này.

| HCLK khi chạy (MHz) | Thời điểm đo | U (V) | I (mA) | P đo (mW) | P hiệu chỉnh (mW) |
| ---: | --- | ---: | ---: | ---: | ---: |
| 16 | Run, cả hai LED sáng | … | … | … | … |
| 16 | Run, cả hai LED tắt | … | … | … | … |
| 16 | Standby | … | … | … | … |
| 96 | Run, cả hai LED sáng | … | … | … | … |
| 96 | Run, cả hai LED tắt | … | … | … | … |
| 96 | Standby | … | … | … | … |
| 180 | Run, cả hai LED sáng | … | … | … | … |
| 180 | Run, cả hai LED tắt | … | … | … | … |
| 180 | Standby | … | … | … | … |

| Thao tác kiểm tra | Trạng thái LED khi CPU chạy | Thời gian đến khi công suất giảm (s) | Diễn biến công suất quan sát được |
| --- | --- | ---: | --- |
| Cấp nguồn lần đầu | … | … | … |
| Từ Standby, nhấn rồi thả PA0 lần thứ nhất | … | … | … |
| Từ Standby, nhấn rồi thả PA0 lần thứ hai | … | … | … |

## 3.5. Bài 5 – Kiểm tra đồng hồ số

Ghi kết quả chạy dự án DigitalClock trên bo. Với các phép thử thời gian, tính từ thời điểm cấp nguồn, chuyển chế độ hoặc nhấn nút như mô tả ở từng hàng.

| Phép thử | Yêu cầu cần kiểm tra | Kết quả quan sát trên bo |
| --- | --- | --- |
| Chạy dự án ban đầu rồi nhấn reset | So sánh ngày giờ hiển thị trước và sau reset. | Trước reset … / Sau reset … |
| Cấp nguồn lần đầu cho chương trình đã sửa | RTC nhận ngày giờ khởi tạo. LCD hiển thị ngày giờ ở chế độ Run và cập nhật mỗi giây. | Ngày giờ ban đầu … / Chu kỳ cập nhật … s |
| Để bo ở Run, không nhấn nút | Sau 10 giây, CPU chuyển sang Sleep. LCD vẫn cập nhật mỗi giây. | Thời gian đến Sleep … s / Chu kỳ cập nhật trong Sleep … s |
| Nhấn PA0 khi đang ở Run | CPU tiếp tục chạy. Khoảng chờ 10 giây được tính lại từ lần nhấn. | Chế độ sau khi nhấn … / Thời gian từ lúc nhấn đến Sleep … s |
| Nhấn PA0 khi đang ở Sleep | CPU trở lại Run và bắt đầu lại khoảng chờ 10 giây. | Chế độ sau khi nhấn … / Thời gian từ lúc nhấn đến Sleep tiếp theo … s |
| Giữ CPU ở Sleep hơn 10 giây | Trước khi vào Standby, LCD hiển thị hai dòng “Standby..” và “Press PA0 to wake up”. | Thời gian từ Sleep đến Standby … s / Dòng 1 … / Dòng 2 … |
| Nhấn PA0 khi đang ở Standby | CPU trở lại Run. LCD hiển thị ngày giờ hiện tại mà RTC đã tiếp tục đếm trong Standby. | Chế độ sau khi nhấn … / Nội dung LCD … |

Dùng các mốc dưới đây để kiểm tra RTC có giữ đúng ngày giờ qua lần thức dậy hay không. Ghi mốc trước Standby ở lần cập nhật cuối cùng trước khi thông báo Standby xuất hiện.

| Ngày giờ trước Standby | Thời gian thực tế giữa hai lần đọc (s) | Ngày giờ sau khi thức dậy bằng PA0 | Nhận xét về tính liên tục của RTC |
| --- | ---: | --- | --- |
| … | … | … | … |
