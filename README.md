# DỰ ÁN QUẢN LÝ TÀI KHOẢN

## Các thành viên tham gia:
- Trần Lê Anh Tú (MSSV: K24DTCN243)
- Kim Ngọc Thành (MSSV: K24DTCN231)
- Phan Huy Cương (MSSV: K24DTCN174)
(phân công nhiệm vụ cụ thể ở phía cuối cùng của file)

## Tổng quan
Ứng dụng Quản lý Tài khoản được phát triển bằng C++, cung cấp các tính năng quản lý tài khoản người dùng và ví điện tử, bao gồm xác thực người dùng, quản lý ví và theo dõi giao dịch.

## Tính năng
- Quản lý tài khoản người dùng (đăng ký, đăng nhập, phân quyền)
- Quản lý ví điện tử (chuyển điểm, theo dõi giao dịch)
- Xác thực bảo mật (OTP, quản lý mật khẩu)

## Kiến trúc hệ thống

### Các module chính
1. Quản lý tài khoản (`AccountSystem.h/cpp`):
   - Đăng ký tài khoản
   - Quản lý thông tin người dùng
   - Phân quyền người dùng

2. Xác thực (`AuthManager.h/cpp`):
   - Hệ thống đăng nhập
   - Tạo và xác thực OTP
   - Quản lý mật khẩu

3. Quản lý dữ liệu (`DataManager.h/cpp`):
   - Lưu trữ thông tin người dùng
   - Quản lý dữ liệu giao dịch
   - Sao lưu và phục hồi dữ liệu

4. Quản lý ví (`WalletManager.h/cpp`):
   - Quản lý số dư
   - Thực hiện giao dịch
   - Theo dõi lịch sử giao dịch

## Hướng dẫn cài đặt và thiết lập

### Yêu cầu hệ thống
- Hệ điều hành: Windows
- Dev-C++ IDE (phiên bản 5.11 trở lên)
- Trình biên dịch MinGW GCC

### Các bước cài đặt
1. Cài đặt Dev-C++
2. Tải về hoặc clone dự án
3. Mở file `AccountManager.dev` bằng Dev-C++

### Biên dịch
1. Trong Dev-C++, chọn Execute > Compile & Run (hoặc nhấn F11)
2. File thực thi sẽ được tạo tự động

### Cấu trúc dự án
Account-Manager/
├── AccountManager.dev    # File dự án Dev-C++
├── AccountSystem.cpp/h   # Quản lý tài khoản
├── AuthManager.cpp/h     # Xác thực
├── DataManager.cpp/h     # Quản lý dữ liệu
├── User.cpp/h           # Định nghĩa người dùng
├── Wallet.cpp/h         # Định nghĩa ví
├── WalletManager.cpp/h  # Quản lý ví
├── main.cpp             # File chính
└── data/               # Thư mục dữ liệu

## Hướng dẫn sử dụng

### Bắt đầu
1. Khởi động: Chạy file thực thi sau khi biên dịch

2. Đăng ký tài khoản:
   - Chọn tùy chọn đăng ký
   - Nhập thông tin cá nhân
   - Hệ thống sẽ tạo mật khẩu tự động

3. Đăng nhập:
   - Nhập tên đăng nhập và mật khẩu
   - Xác thực OTP nếu được yêu cầu

### Chức năng người dùng thông thường
- Xem thông tin cá nhân
- Quản lý ví
- Thực hiện giao dịch
- Xem lịch sử giao dịch

### Chức năng người quản lý
- Tất cả chức năng của người dùng thông thường
- Quản lý danh sách nhóm
- Tạo tài khoản mới
- Hỗ trợ điều chỉnh thông tin tài khoản

### Quản lý ví
- Kiểm tra số dư
- Chuyển điểm
- Xem lịch sử giao dịch
- Theo dõi trạng thái giao dịch

### Lưu ý quan trọng
- Dữ liệu được lưu trong thư mục `data/`
- Không chỉnh sửa hoặc xóa các file trong thư mục dữ liệu
- Nên sao lưu dữ liệu thường xuyên

## Phát triển
Dự án được phát triển bằng IDE Dev-C++ với các thư viện C++ chuẩn.

## Phân công:

### Trần Lê Anh Tú
(***Trong quá trình thực hiện, do có thành viên sơ suất chạy lệnh git push --force nên làm mất commit 'upload project: create system and classes' trong lịch sử.
Thầy vui lòng truy cập https://github.com/TranTu-2k/Account-Manager/activity để xem đầy đủ lịch sử commit. Em xin cảm ơn!)

Khởi tạo các lớp với một số chức năng cơ bản:
1. Lớp Chính (Core Classes)
* AccountSystem
- Vai trò : Class chính điều phối toàn bộ hệ thống
- Thành phần : AuthManager, DataManager, WalletManager
- Chức năng : Quản lý người dùng, ví, xác thực và phân quyền
* AuthManager
- Vai trò : Quản lý xác thực và bảo mật
- Chức năng chính :
  - Đăng ký/đăng nhập
  - Quản lý mật khẩu và OTP
  - Phân quyền người dùng
* DataManager
- Vai trò : Quản lý dữ liệu và lưu trữ
- Dữ liệu quản lý : Users, Wallets, Transactions
- Chức năng chính :
  - CRUD cho các đối tượng
  - Sao lưu và phục hồi
  - Quản lý file dữ liệu
* WalletManager
- Vai trò : Quản lý ví và giao dịch
- Chức năng chính :
  - Tạo và quản lý ví
  - Xử lý giao dịch
  - Theo dõi lịch sử
2. Lớp Đối tượng (Entity Classes)
* User
- Thông tin cá nhân và xác thực
- Phân quyền (Regular/Admin)
- Theo dõi trạng thái đăng nhập
* Wallet
- Thông tin ví và số dư
- Quản lý giao dịch
- Lịch sử giao dịch
* Transaction
- Thông tin giao dịch
- Lưu trạng thái và thời gian
- Mô tả giao dịch
* OTP
- Mã xác thực
- Thời gian hiệu lực
- Quản lý mục đích sử dụng

### Kim Ngọc Thành

Thêm và hoàn thiện một số tính năng:

* Giao diện người dùng :
- Hiển thị menu chính (showMainMenu)
- Hiển thị menu ví (showWalletMenu)
- Xóa màn hình (clearScreen) - hỗ trợ cả Windows và các hệ điều hành khác
- Xử lý buffer đầu vào (clearInputBuffer)

* Quản lý tài khoản cơ bản :
- Đăng ký người dùng (registerUser)
- Đăng nhập (login)
- Đổi mật khẩu (changePassword)
- Xem thông tin cá nhân (viewProfile)

* Chức năng quản trị viên :
- Đăng ký tài khoản mới cho người dùng (registerUserByAdmin)
- Xem danh sách tất cả người dùng (viewAllUsers)
- Đặt lại mật khẩu cho người dùng (resetUserPassword)

* Quản lý ví :
- Xem số dư ví (viewWalletBalance)
- Hiển thị thông tin ví trong profile

* Phân quyền người dùng :
- Phân biệt menu cho người dùng thường và admin
- Kiểm tra trạng thái đăng nhập
- Kiểm tra quyền admin

* Hiển thị thông tin :
- Hiển thị thông tin người dùng chi tiết
- Hiển thị thông tin ví
- Hiển thị trạng thái các thao tác (thành công/thất bại)

* Chức năng chuyển điểm ( transferPoints ):
- Nhập ID ví người nhận
- Nhập số điểm cần chuyển
- Thêm mô tả cho giao dịch
- Xác thực bằng OTP:
  - Tự động sinh mã OTP
  - Yêu cầu nhập mã xác thực
- Thông báo kết quả giao dịch

* Quản lý thao tác ví ( handleWalletOperations ):
- Menu quản lý ví với các chức năng:
  - Xem số dư
  - Xem lịch sử giao dịch
  - Chuyển điểm
- Xử lý lựa chọn người dùng
- Giao diện tương tác

* Chương trình chính ( main ):
- Khởi động hệ thống
- Menu chính phân quyền:
  Khi chưa đăng nhập :
  - Thoát chương trình
  - Đăng nhập
  - Đăng ký
  Khi đã đăng nhập :
  - Đăng xuất
  - Xem thông tin cá nhân
  - Đổi mật khẩu
  - Quản lý ví
  Chức năng Admin :
  - Xem danh sách người dùng
  - Đăng ký người dùng mới
  - Đặt lại mật khẩu người dùng

* Một số tính năng khác:
- Xử lý lựa chọn không hợp lệ
- Tạm dừng màn hình để đọc thông báo
- Xóa màn hình để tăng tính thẩm mỹ
- Xử lý buffer đầu vào

### Phan Huy Cương

Thêm và hoàn thiện một số tính năng:

* Xác thực hai yếu tố (2FA)
- Thêm menu quản lý 2FA:
  + Bật/tắt 2FA
  + Kiểm tra 2FA
  + Xác thực mã TOTP
- Tích hợp 2FA vào đăng nhập
- Xử lý đăng xuất khi xác thực thất bại

* Quản lý mật khẩu tự động
- Phát hiện và bắt buộc đổi mật khẩu tự động
- Tích hợp xác thực OTP khi đổi mật khẩu
- Tự động đăng xuất khi thất bại

* Cập nhật thông tin người dùng
- Cho người dùng thường:
  + Cập nhật thông tin cá nhân (họ tên, email, số điện thoại)
  + Xác thực OTP trước khi cập nhật
  + Giữ thông tin cũ nếu để trống

- Cho Admin:
  + Cập nhật thông tin người dùng khác
  + Đặt lại mật khẩu với xác thực OTP
  + Xem danh sách người dùng thường

* Quản lý ví điện tử
- Nạp tiền vào ví (Admin):
  + Tự động tạo ví mới
  + Kiểm tra số tiền hợp lệ
  + Xác thực OTP

- Cải thiện chức năng chuyển điểm:
  + Xác thực OTP hai bước
  + Hiển thị danh sách ví
  + Thông báo chi tiết trạng thái

* Nâng cấp hiển thị thông tin người dùng
- Hiển thị chi tiết:
  + Thông tin cơ bản
  + Thời gian tạo tài khoản và đăng nhập
  + Thông tin ví
- Định dạng hiển thị danh sách người dùng

* Lịch sử giao dịch
- Tổng quan giao dịch
- Chi tiết từng giao dịch
- Định dạng bảng hiển thị (Index, Date, Amount, Direction, Status)

* Cải thiện giao diện và xử lý
- Tối ưu xử lý đầu vào với getline
- Xóa màn hình tự động
- Thông báo chi tiết cho mỗi thao tác
- Xác nhận người dùng trước khi thay đổi
