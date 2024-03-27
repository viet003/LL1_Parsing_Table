#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <stack>

using namespace std;

void find_first(vector< pair<char, string> > gram, 
	map< char, set<char> > &firsts, 
	char non_term); 

void find_follow(vector< pair<char, string> > gram, 
	map< char, set<char> > &follows, 
	map< char, set<char> > firsts, 
	char non_term); 


int main(int argc, char const *argv[])
{

	// Đọc dữ liệu từ file được truyền vào
	fstream file;
	file.open(argv[1], ios::in);
	if(file.fail()) {
		// in ra nếu có lỗi trong quá trình mở tệp
		cout<<"Lỗi không thể mở file\n";
		return 2;
	}

	cout<<"Ngữ pháp được đọc vào từ tệp ngữ pháp: \n";
	// Khởi tạo vactor lưu trữ ngữ pháp
	vector< pair<char, string> > gram;
	int count = 0;
	while(!file.eof()) {
		char buffer[20];
		file.getline(buffer, 19);

		char lhs = buffer[0];
		string rhs = buffer+3;
		// tạo một pair đại diện cho một quy tắc ngữ pháp.
		pair <char, string> prod (lhs, rhs);
		gram.push_back(prod);
		// in ra màn mình các quy tắc ngữ pháp kèm với chỉ mục
		cout<<count++<<".  "<<gram.back().first<<" -> "<<gram.back().second<<"\n";
	}
	cout<<"\n";

	// khởi tạo set lưu trữ các ký tự không kết thúc
	set<char> non_terms;
	for(auto i = gram.begin(); i != gram.end(); ++i) {
		non_terms.insert(i->first);
	}
	// in ra các ký tự không kết thúc
	cout<<"Ky tu khong ket thuc: ";
	for(auto i = non_terms.begin(); i != non_terms.end(); ++i) {
		cout<<*i<<" ";
	}
	cout<<"\n";


	// khởi tạo set lưu trữ ký tự kết thúc
	set<char> terms;
	for(auto i = gram.begin(); i != gram.end(); ++i) {
		for(auto ch = i->second.begin(); ch != i->second.end(); ++ch) {
			if(!isupper(*ch)) {
				terms.insert(*ch);
			}
		}
	}
	// Loại bỏ epsilon và thêm ký tự kết thúc $
	terms.erase('e');
	terms.insert('$');
	// in ra các ký tự kết thúc
	cout<<"Ky tu ket thuc: ";
	for(auto i = terms.begin(); i != terms.end(); ++i) {
		cout<<*i<<" ";
	}
	cout<<"\n\n";


	// xác định ký tự bắt đầu bằng cách lấy biến phi chữ số đầu tiên từ ngữ pháp.
	char f_start = gram.begin()->first;
	// khởi tạo một map lưu trữ first.
	map< char, set<char> > firsts;
	for(auto non_term = non_terms.begin(); non_term != non_terms.end(); ++non_term) {
		// nếu chưa tính first, gọi hàm tính và lưu giá trị vào map
		if(firsts[*non_term].empty()){
			find_first(gram, firsts, *non_term);
		}
	}
	// in ra màn hình.
	cout<<"Firsts: \n";
	for(auto it = firsts.begin(); it != firsts.end(); ++it) {
		cout<<it->first<<" : ";
		for(auto firsts_it = it->second.begin(); firsts_it != it->second.end(); ++firsts_it) {
			cout<<*firsts_it<<" ";
		}
		cout<<"\n";
	}
	cout<<"\n";



	map< char, set<char> > follows;
	// xác định biến bắt đầu bằng cách lấy biến phi chữ số đầu tiên từ ngữ pháp.
	char fl_start = gram.begin()->first;
	follows[fl_start].insert('$');
	find_follow(gram, follows, firsts, fl_start);
	// Tìm follow cho phần còn lại.
	for(auto it = non_terms.begin(); it != non_terms.end(); ++it) {
		// Nếu chưa tính follow, gọi hàm find_follow và thực hiện tính toán
		if(follows[*it].empty()) {
			find_follow(gram, follows, firsts, *it);
		}
	}
	// in ra màn hình.
	cout<<"Follows: \n";
	for(auto it = follows.begin(); it != follows.end(); ++it) {
		cout<<it->first<<" : ";
		for(auto follows_it = it->second.begin(); follows_it != it->second.end(); ++follows_it) {
			cout<<*follows_it<<" ";
		}
		cout<<"\n";
	}
	cout<<"\n";

	// khởi tạo một ma trận hai chiều có kích thước phù hợp -> bảng phân tích ngữ pháp
	int parse_table[non_terms.size()][terms.size()];
	// lấp đầy bảng bằng cách thêm phần tử -1
	fill(&parse_table[0][0], &parse_table[0][0] + sizeof(parse_table)/sizeof(parse_table[0][0]), -1);
	// Tạo bảng parse_table
    for(auto i = gram.begin(); i != gram.end(); ++i) {
        string rhs = i->second;
		// khởi tạo một set lưu trữ	các rhs của quy tắc
        set<char> list;
        bool finished = false;
		// duyệt tất cả phần tử trong rhs
        for(auto ch = rhs.begin(); ch != rhs.end(); ++ch) {
			// kiểm tra xem ký tự không phải viết hoa và khác epsilon
			// nếu đúng thì insert vào list
            if(!isupper(*ch)) {
                if(*ch != 'e') {
                    list.insert(*ch);
                    finished = true;
                    break;
                }
                continue;
            }
			// tạo một set bao gồm tất cả các first
            set<char> firsts_copy(firsts[*ch].begin(), firsts[*ch].end());
            if(firsts_copy.find('e') == firsts_copy.end()) {
                list.insert(firsts_copy.begin(), firsts_copy.end());
                finished = true;
                break;
            }
			// loại bỏ epsilon
            firsts_copy.erase('e');
			// thêm vào list
            list.insert(firsts_copy.begin(), firsts_copy.end());
        }
        // Nếu toàn bộ rhs có thể bị bỏ qua thông qua epsilon hoặc đạt đến cuối
        // Thêm follow vào list
        if(!finished) {
            list.insert(follows[i->first].begin(), follows[i->first].end());
        }

        for(auto ch = list.begin(); ch != list.end(); ++ch) {
			// lấy ra địa chỉ hàng
            int row = distance(non_terms.begin(), non_terms.find(i->first));
			// lấy ra địa chỉ cột
            int col = distance(terms.begin(), terms.find(*ch));
			// lấy ra quy tắc
            int i_num = distance(gram.begin(), i);
            if(parse_table[row][col] != -1) {
				// xử lý xung đột
                cout<<"Xung đột tại ["<<row<<"]["<<col<<"] cho quy tắc "<<i_num<<"\n";
                continue;
            }
			// cập nhật giá trị trong parse_table
            parse_table[row][col] = i_num;
        }
    }
	
	// in ra màn hình bảng phân tích cú pháp
	cout<<"Bảng phân tích cú pháp: \n";
	cout<<"   ";
	for(auto i = terms.begin(); i != terms.end(); ++i) {
		cout<<*i<<" ";
	}
	cout<<"\n";
	for(auto row = non_terms.begin(); row != non_terms.end(); ++row) {
		cout<<*row<<"  ";
		for(int col = 0; col < terms.size(); ++col) {
			int row_num = distance(non_terms.begin(), row);
			if(parse_table[row_num][col] == -1) {
				cout<<"- ";
				continue;
			}
			cout<<parse_table[row_num][col]<<" ";
		}
		cout<<"\n";
	}
	cout<<"\n";

	return 0;
}

void find_first(vector< pair<char, string> > gram, 
	map< char, set<char> > &firsts, 
	char non_term) {

	for(auto i = gram.begin(); i != gram.end(); ++i) {
		// Tìm các quy tắc sản xuất của ký tự không k
		if(i->first != non_term) {
			continue;
		}
		string rhs = i->second;
		// Lặp cho đến khi gặp một ký tự không k hoặc không có biến epsilon
		for(auto ch = rhs.begin(); ch != rhs.end(); ++ch) {
			// Nếu ký tự đầu tiên trong quy tắc là ký tự không k, thêm vào tập hợp firsts
			if(!isupper(*ch)) {
				firsts[non_term].insert(*ch);
				break;
			}
			else {
				// Nếu ký tự trong quy tắc là ký tự không k và tập hợp firsts của nó chưa được tính toán
				// Tính toán tập hợp firsts cho ký tự không k đó
				if(firsts[*ch].empty()) {
					find_first(gram, firsts, *ch);
				}
				// Nếu biến không chứa epsilon, dừng vòng lặp
				if(firsts[*ch].find('e') == firsts[*ch].end()) {
					firsts[non_term].insert(firsts[*ch].begin(), firsts[*ch].end());
					break;
				}

				set<char> firsts_copy(firsts[*ch].begin(), firsts[*ch].end());

				// Loại bỏ epsilon từ tập hợp firsts nếu không phải là biến cuối cùng
				if(ch + 1 != rhs.end()) {
					firsts_copy.erase('e');
				}

				// Thêm tập hợp firsts của biến đó vào tập hợp firsts của biến không k
				firsts[non_term].insert(firsts_copy.begin(), firsts_copy.end());
			}
		}
		
	}
}


void find_follow(vector< pair<char, string> > gram, 
	map< char, set<char> > &follows, 
	map< char, set<char> > firsts, 
	char non_term) {


	for(auto it = gram.begin(); it != gram.end(); ++it) {

		// Biến finished là true khi tìm follow từ quy tắc sản xuất này đã hoàn thành
		bool finished = true;
		auto ch = it->second.begin();

		// Bỏ qua các biến cho đến khi gặp ký tự không k cần thiết
		for(;ch != it->second.end() ; ++ch) {
			if(*ch == non_term) {
				finished = false;
				break;
			}
		}
		++ch;

		for(;ch != it->second.end() && !finished; ++ch) {
			// Nếu là ký tự không kết thúc, chỉ cần thêm vào follow
			if(!isupper(*ch)) {
				follows[non_term].insert(*ch);
				finished = true;
				break;
			}

			set<char> firsts_copy(firsts[*ch]);
			// Nếu tập hợp firsts của ký tự không chứa epsilon, dừng việc tìm kiếm follow
			if(firsts_copy.find('e') == firsts_copy.end()) {
				follows[non_term].insert(firsts_copy.begin(), firsts_copy.end());
				finished = true;
				break;
			}
			// Ngược lại, ký tự tiếp theo phải được kiểm tra sau khi thêm firsts vào follow
			firsts_copy.erase('e');
			follows[non_term].insert(firsts_copy.begin(), firsts_copy.end());

		}


		// Nếu là cuối của quy tắc sản xuất, follow giống như follow của biến
		if(ch == it->second.end() && !finished) {
			// Tìm follow nếu chưa có
			if(follows[it->first].empty()) {
				find_follow(gram, follows, firsts, it->first);
			}
			follows[non_term].insert(follows[it->first].begin(), follows[it->first].end());
		}

	}

}
