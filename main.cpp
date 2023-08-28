#include <bits/stdc++.h>
#include<ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <windows.h>

using namespace ftxui;
using namespace std;

string test	  = "";	       // 提示信息
string user	  = "";
int next_idx_sale = 1;	       // 下一个点菜编号

struct dish_base	       // 菜肴基本信息
{
	string name;	       // 菜肴名称
	string type;	       // 菜肴种类
	string ingredients;    // 菜肴配料
	double price;	       // 菜肴单价
	string other;	       // 备注
	dish_base *next;       // 指向下一个菜肴信息
	// 清空该节点信息
	void clear() {
		name        = "";
		type        = "";
		ingredients = "";
		price       = 0;
		other       = "";
	}
	// 删除节点，用于释放内存
	void del() {
		if(next != NULL) {
			next->del();
		}
		delete this;
	}
};
struct dish_sale	         // 顾客点菜信息
{
	string idx;	         // 桌位号
	string name;	         // 菜肴名称
	string idx_customer;     // 顾客编号
	string name_customer;    // 顾客名称
	string name_waiter;      // 服务员名称
	string time;	         // 点菜时间
	double num;	         // 数量
	double price;	         // 单价
	double sum;	         // 总价
	int idx_sale;	         // 点菜编号
	dish_sale *next;         // 指向下一个点菜信息
	// 清空该节点信息
	void clear() {
		idx           = "";
		name          = "";
		idx_customer  = "";
		name_customer = "";
		name_waiter   = "";
		time          = "";
		price         = 0;
		sum           = 0;
	}
	// 删除节点，用于释放内存
	void del() {
		if(next != NULL) {
			next->del();
		}
		delete this;
	}
};
struct check_out	        // 顾客买单信息
{
	string idx;	        // 桌位号
	string idx_customer;    // 顾客编号
	double sum;	        // 消费金额
	string time;	        // 点菜时间
	string name_waiter;     // 服务员名称
	int idx_sale;	        // 点菜编号
	string is;	        // 是否结账
	check_out *next;        // 指向下一个菜肴信息
	// 清空该节点信息
	void clear() {
		idx          = "";
		idx_customer = "";
		sum          = 0;
		time         = "";
		name_waiter  = "";
		is           = "";
	}
	// 删除节点，用于释放内存
	void del() {
		if(next != NULL) {
			next->del();
		}
		delete this;
	}
};

map< string, string > map_account;	// 存储账号
map< string, dish_base * > map_dish_base;	// 存储菜肴
map< int, dish_sale * > map_dish_sale;	// 存储点菜信息
map< int, check_out * > map_check_out;	// 存储买单信息

dish_base *dish_base_root = new dish_base();	// 菜肴基本信息表头
dish_sale *dish_sale_root = new dish_sale();	// 顾客点菜信息表头
check_out *check_out_root = new check_out();	// 顾客买单信息表头

void loading();
void save();
void menu_login();
void menu_register();
void menu_change();
void menu();
void in();
void order();
void pay();
void change_dish(string name);
void in_change_dish();
void print();
void check_sale();
void check_base();
void check_check();
void vount_base();
void vount_sale();
void vount_check();
void summary();
double to_double(string s);

// 将string转为double
double to_double(string s) {
	stringstream ss(s);
	double sum = 0;
	ss >> sum;
	return sum;
}

// 将double转为string
string tostring(double n) {
	string s = to_string(n);
	while(s.back() == '0')
		s.pop_back();	 // 去除无效位
	if(s.back() == '.')
		s.pop_back();
	return s;
}

// 预加载保存的文件信息
void loading() {
	string s1, s2;
	int a, b;
	fstream in;
	{    // 加载账号
		in.open("data/account.dat", ios::in);
		while(in >> s1 >> s2) {
			map_account[s1] = s2;
		}
		in.close();
	}

	if(user.empty())
		return;

	{    // 加载菜肴基本信息
		in.open("data/" + user + "/dish_base.dat", ios::in);
		dish_base *root = dish_base_root;
		while(in >> root->name) {
			map_dish_base[root->name] = root;    // 记录菜肴
			in >> root->type >> root->price >> root->other;
			getline(in, root->ingredients);
			root->ingredients.erase(root->ingredients.begin());
			root->next = new dish_base();
			root       = root->next;
		}
		in.close();
	}

	{    // 加载点菜信息
		in.open("data/" + user + "/dish_sale.dat", ios::in);
		dish_sale *root = dish_sale_root;
		while(in >> root->name) {
			in >> root->idx >> root->idx_customer >> root->name_customer >> root->num
			    >> root->price >> root->sum >> root->time >> root->name_waiter >> root->idx_sale;

			map_dish_sale[root->idx_sale] = root;
			next_idx_sale	              = max(root->idx_sale + 1, next_idx_sale);    // 记录下一个点菜编号

			root->next = new dish_sale();
			root       = root->next;
		}
		in.close();
	}

	{    // 加载买单信息
		in.open("data/" + user + "/check-out.dat", ios::in);
		check_out *root = check_out_root;
		while(in >> root->idx) {
			in >> root->idx_customer >> root->sum >> root->time >> root->name_waiter >> root->is >> root->idx_sale;
			map_check_out[root->idx_sale] = root;
			root->next	              = new check_out();
			root	              = root->next;
		}
		in.close();
	}
}

// 将信息保存到文件中
void save() {
	fstream out;
	{    // 保存账号
		out.open("data/account.dat", ios::out);
		for(auto it = map_account.begin(); it != map_account.end(); it++) {
			if(it->second != "") {
				out << it->first << " " << it->second << endl;
			}
		}
		out.close();
	}

	if(user.empty())
		return;

	{    // 保存菜肴基本信息
		out.open("data/" + user + "/dish_base.dat", ios::out);
		dish_base *root = dish_base_root;
		while(root->next) {
			out << root->name << " " << root->type << " " << root->price << " " << root->other
			    << " " << root->ingredients << "\n";
			root = root->next;    // 下一菜肴
		}
		out.close();
	}

	{    // 保存点菜信息
		out.open("data/" + user + "/dish_sale.dat", ios::out);
		dish_sale *root = dish_sale_root;
		while(root->next) {
			out << root->name << " " << root->idx << " " << root->idx_customer << " " << root->name_customer << " " << root->num << " " << root->price << " " << root->sum << " " << root->time << " " << root->name_waiter << " " << root->idx_sale << "\n";
			root = root->next;    // 下一菜肴
		}
		out.close();
	}

	{    // 保存买单信息
		out.open("data/" + user + "/check-out.dat", ios::out);
		check_out *root = check_out_root;
		while(root->next) {
			out << root->idx << " " << root->idx_customer << " " << root->sum << " " << root->time << " " << root->name_waiter << " " << root->is << " " << root->idx_sale << "\n";
			root = root->next;
		}
		out.close();
	}
}

// 录入菜肴基本信息
void in() {
	dish_base *root = dish_base_root;
	while(root->next)    // 找到链表末尾
	{
		root = root->next;
	}
	test = "";
	root->clear();	  // 清空缓存信息
	string temp;
	auto screen	            = ScreenInteractive::TerminalOutput();
	Component input_name        = Input(&root->name, "请输入菜肴名称");
	Component input_type        = Input(&root->type, "请输入菜肴种类");
	Component input_price       = Input(&temp, "请输入菜肴单价");
	Component input_other       = Input(&root->other, "请输入备注");
	Component input_ingredients = Input(&root->ingredients, "请输入菜肴配料");
	auto quit	            = Button("退出", screen.ExitLoopClosure());
	auto enter	            = Button("确认", [&] {
		 if(map_dish_base[root->name]) {
			 test = "已有相同名称的菜肴";
		 } else if(root->name.empty() || root->type.empty() || temp.empty() || root->other.empty() || root->ingredients.empty()) {
			 test = "请输入完整信息";
		 } else {
			 root->price	           = to_double(temp);
			 map_dish_base[root->name] = root;
			 root->next	           = new dish_base();
			 save();
			 screen.Exit();
		 }
	 });

	auto comp = Container::Horizontal({
	    // 组件列表
	    input_name,
	    input_type,
	    input_price,
	    input_other,
	    input_ingredients,
	    enter,
	    quit,
	});

	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		return vbox({
		           text("餐饮管理系统") | center,
		           separator(),
		           hbox(text("菜肴名称"), separator(), input_name->Render()),
		           separator(),
		           hbox(text("菜肴种类"), separator(), input_type->Render()),
		           separator(),
		           hbox(text("菜肴单价"), separator(), input_price->Render()),
		           separator(),
		           hbox(text("    备注"), separator(), input_other->Render()),
		           separator(),
		           hbox(text("菜肴配料"), separator(), input_ingredients->Render()),
		           separator(),
		           hbox(enter->Render(), separatorEmpty(), quit->Render()) | center,
		           text(test) | color(Color::Red),
		       })
		       | border | size(WIDTH, EQUAL, 40) | center;
	});
	screen.Loop(renderer);
}

// 顾客点菜
void order() {
	dish_sale *root = dish_sale_root;
	while(root->next)    // 找到链表末尾
	{
		root = root->next;
	}
	test = "";
	root->clear();	  // 清空缓存信息
	auto screen	              = ScreenInteractive::TerminalOutput();
	Component input_name          = Input(&root->name, "请输入菜肴名称");
	Component input_idx           = Input(&root->idx, "请输入桌位号");
	Component input_idx_customer  = Input(&root->idx_customer, "请输入顾客编号");
	Component input_name_customer = Input(&root->name_customer, "请输入顾客名称");
	Component input_time          = Input(&root->time, "请输入点菜时间");
	Component input_name_waiter   = Input(&root->name_waiter, "请输入服务员名称");
	auto quit	              = Button("退出", screen.ExitLoopClosure());
	auto enter	              = Button("确认", [&] {
		   if(!map_dish_base[root->name]) {
			   test = "无此菜肴信息";
		   } else if(root->name.empty() || root->idx.empty() || root->idx_customer.empty() || root->name_customer.empty() || root->time.empty() || root->name_waiter.empty()) {
			   test = "请输入完整信息";
		   } else {
			   root->num	  = 1;
			   root->price	  = map_dish_base[root->name]->price;
			   root->sum	  = root->price * root->num;
			   root->idx_sale = next_idx_sale++;    // 记录点菜编号

			   // 生成买单信息
			   check_out *root_check = check_out_root;
			   while(root_check->next)    // 找到链表末尾
			   {
				   root_check = root_check->next;
			   }

			   map_dish_sale[root->idx_sale] = root;
			   map_check_out[root->idx_sale] = root_check;

			   root_check->idx          = root->idx;
			   root_check->idx_customer = root->idx_customer;
			   root_check->idx_sale     = root->idx_sale;
			   root_check->name_waiter  = "暂无";
			   root_check->sum          = root->sum;
			   root_check->time         = root->time;
			   root_check->is           = "否";

			   root_check->next = new check_out();    // 创建新的买单信息
			   root->next	    = new dish_sale();    // 创建新的点菜信息

			   save();
			   screen.Exit();
		   }
	   });

	auto comp = Container::Horizontal({
	    // 组件列表
	    input_name,
	    input_idx,
	    input_idx_customer,
	    input_name_customer,
	    input_time,
	    input_name_waiter,
	    enter,
	    quit,
	});

	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		return vbox({
		           text("餐饮管理系统") | center,
		           separator(),
		           hbox(text("  菜肴名称"), separator(), input_name->Render()),
		           separator(),
		           hbox(text("    桌位号"), separator(), input_idx->Render()),
		           separator(),
		           hbox(text("  顾客编号"), separator(), input_idx_customer->Render()),
		           separator(),
		           hbox(text("  顾客名称"), separator(), input_name_customer->Render()),
		           separator(),
		           hbox(text("  点菜时间"), separator(), input_time->Render()),
		           separator(),
		           hbox(text("服务员名称"), separator(), input_name_waiter->Render()),
		           separator(),
		           hbox(enter->Render(), separatorEmpty(), quit->Render()) | center,
		           text(test) | color(Color::Red),
		       })
		       | border | size(WIDTH, EQUAL, 40) | center;
	});
	screen.Loop(renderer);
}

// 顾客买单
void pay() {
	check_out in;
	// root->clear();//清空缓存信息

	auto screen	             = ScreenInteractive::TerminalOutput();
	Component input_idx          = Input(&in.idx, "请输入桌位号");
	Component input_idx_customer = Input(&in.idx_customer, "请输入顾客编号");
	Component input_time         = Input(&in.time, "请输入点菜时间");
	Component input_name_waiter  = Input(&in.name_waiter, "请输入服务员名称");
	auto quit	             = Button("退出", screen.ExitLoopClosure());
	auto enter	             = Button("确认", [&] {
		  if(in.idx.empty() || in.idx_customer.empty() || in.time.empty() || in.name_waiter.empty()) {
			  test = "请输入完整信息";
			  return;
		  }
		  dish_sale *root = dish_sale_root;
		  int idx_sale	  = 0;
		  while(root->next)    // 检查信息是否对应
		  {
			  if(root->idx == in.idx && root->idx_customer == in.idx_customer && root->time == in.time) {
				  idx_sale = root->idx_sale;
				  break;
			  }
			  root = root->next;
		  }

		  if(!idx_sale) {
			  test = "无此顾客点菜信息";
		  } else if(map_check_out[idx_sale]->is == "是") {
			  test = "顾客已买单";
		  } else {
			  map_check_out[idx_sale]->is	       = "是";
			  map_check_out[idx_sale]->name_waiter = in.name_waiter;
			  save();
			  screen.Exit();
		  }
	  });

	auto comp = Container::Horizontal({
	    // 组件列表
	    input_idx,
	    input_idx_customer,
	    input_time,
	    input_name_waiter,
	    enter,
	    quit,
	});

	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		return vbox({
		           text("餐饮管理系统") | center,
		           separator(),
		           hbox(text("    桌位号"), separator(), input_idx->Render()),
		           separator(),
		           hbox(text("  顾客编号"), separator(), input_idx_customer->Render()),
		           separator(),
		           hbox(text("  点菜时间"), separator(), input_time->Render()),
		           separator(),
		           hbox(text("服务员名称"), separator(), input_name_waiter->Render()),
		           separator(),
		           hbox(enter->Render(), separatorEmpty(), quit->Render()) | center,
		           text(test) | color(Color::Red),
		       })
		       | border | size(WIDTH, EQUAL, 40) | center;
	});
	screen.Loop(renderer);
}

// 修改菜肴基本信息
void change_dish(string name) {
	dish_base *root = map_dish_base[name];
	test	= "";
	string temp[4];
	temp[0] = root->type;
	temp[1] = tostring(root->price);
	temp[2] = root->other;
	temp[3] = root->ingredients;

	auto screen = ScreenInteractive::TerminalOutput();

	Component input_type        = Input(&temp[0], "");    // 将菜肴信息加载到输入框
	Component input_price       = Input(&temp[1], "");
	Component input_other       = Input(&temp[2], "");
	Component input_ingredients = Input(&temp[3], "");

	auto quit  = Button("退出", screen.ExitLoopClosure());
	auto enter = Button("确认", [&] {
		root->type	  = temp[0];
		root->price	  = to_double(temp[1]);
		root->other	  = temp[2];
		root->ingredients = temp[3];
		test	  = "修改成功";
	});
	auto del   = Button("删除", [&] {
		  dish_sale *search_sale = dish_sale_root;
		  while(search_sale->next) {
			  if(search_sale->name == name) {
				  test = "当前菜肴已有顾客点菜";
				  return;
			  }
			  search_sale = search_sale->next;
		  }

		  dish_base *root = dish_base_root;
		  while(root->next && root->next->name != name)	   // 找到对应菜肴
		  {
			  root = root->next;
		  }
		  root->next	      = root->next->next;
		  map_dish_base[name] = NULL;
		  save();
		  screen.Exit();
	  });

	auto comp = Container::Horizontal({
	    // 组件列表
	    input_type,
	    input_price,
	    input_other,
	    input_ingredients,
	    enter,
	    del,
	    quit,
	});

	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		return vbox({
		           text("餐饮管理系统") | center,
		           separator(),
		           hbox(text("菜肴种类"), separator(), input_type->Render()),
		           separator(),
		           hbox(text("菜肴单价"), separator(), input_price->Render()),
		           separator(),
		           hbox(text("    备注"), separator(), input_other->Render()),
		           separator(),
		           hbox(text("菜肴配料"), separator(), input_ingredients->Render()),
		           separator(),
		           hbox(enter->Render(), separatorEmpty(), del->Render(), separatorEmpty(), quit->Render()) | center,
		           text(test) | color(Color::Red),
		       })
		       | border | size(WIDTH, EQUAL, 40) | center;
	});
	screen.Loop(renderer);
}

// 修改菜肴基本信息输入菜肴名称
void in_change_dish() {
	string name = "";
	test        = "";

	auto screen	     = ScreenInteractive::TerminalOutput();
	Component input_name = Input(&name, "请输入菜肴名称");
	auto quit	     = Button("退出", screen.ExitLoopClosure());
	auto enter	     = Button("确认", [&] {
		          if(map_dish_base[name] == NULL) {
			          test = "无此菜肴信息";
		          } else {
			          change_dish(name);
			          screen.Exit();
		          }
	          });

	auto comp = Container::Horizontal({
	    // 组件列表
	    input_name,
	    enter,
	    quit,
	});

	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		return vbox({
		           text("餐饮管理系统") | center,
		           separator(),
		           hbox(text("菜肴名称"), separator(), input_name->Render()),
		           separator(),
		           hbox(enter->Render(), separatorEmpty(), quit->Render()) | center,
		           text(test) | color(Color::Red),
		       })
		       | border | size(WIDTH, EQUAL, 40) | center;
	});
	screen.Loop(renderer);
}

// 输出全部信息
void print() {

	test         = "";
	int selected = 1;
	int page = 0, max_base = 0, max_sale = 0, max_pay = 0;
	auto screen	 = ScreenInteractive::TerminalOutput();
	auto choose_base = Button("菜肴信息", [&] {selected = 1; page = 0; });
	auto choose_sale = Button("点菜信息", [&] {selected = 2;  page = 0; });
	auto choose_pay	 = Button("买单信息", [&] {selected = 3; page = 0; });
	auto quit	 = Button("退出", screen.ExitLoopClosure());
	auto page_last	 = Button("上一页", [&] {if (page > 0)page--; });
	auto page_next	 = Button("下一页", [&] {
		  if(selected == 1) {
			  if(page < max_base)
				  page++;
		  } else if(selected == 2) {
			  if(page < max_sale)
				  page++;
		  } else {
			  if(page < max_pay)
				  page++;
		  }
	  });
	auto comp	 = Container::Horizontal({
	           // 组件列表
	           choose_base,
	           choose_sale,
	           choose_pay,
	           quit,
	           page_last,
	           page_next,
	       });
	auto renderer	 = Renderer(comp, [&] {    // 渲染菜单
		   Elements el_dish[5], el_sale[7], el_pay[6];
		   // 菜肴信息
		   {
			   dish_base *root = dish_base_root;
			   el_dish[0].push_back(vbox(text("菜肴名称"), separator()));
			   el_dish[1].push_back(vbox(text("菜肴种类"), separator()));
			   el_dish[2].push_back(vbox(text("菜肴单价"), separator()));
			   el_dish[3].push_back(vbox(text("备注"), separator()));
			   el_dish[4].push_back(vbox(text("菜肴配料"), separator()));
			   int now = 0, flag = 1;
			   while(root->next) {
				   if(now / 8 == page) {
					   if(flag) {
						   el_dish[0].push_back(vbox(text(root->name)) | flex);
						   el_dish[1].push_back(vbox(text(root->type)) | flex);
						   el_dish[2].push_back(vbox(text(tostring(root->price))) | flex);
						   el_dish[3].push_back(vbox(text(root->other)) | flex);
						   el_dish[4].push_back(vbox(text(root->ingredients)) | flex);
						   flag = 0;
					   } else {
						   el_dish[0].push_back(vbox(separatorEmpty(), text(root->name)) | flex);
						   el_dish[1].push_back(vbox(separatorEmpty(), text(root->type)) | flex);
						   el_dish[2].push_back(vbox(separatorEmpty(), text(tostring(root->price))) | flex);
						   el_dish[3].push_back(vbox(separatorEmpty(), text(root->other)) | flex);
						   el_dish[4].push_back(vbox(separatorEmpty(), text(root->ingredients)) | flex);
					   }
				   }
				   root = root->next;
				   now++;
			   }
			   max_base = max((now - 1) / 8, max_base);
			   if(selected == 1)
				   test = "第" + tostring(page + 1) + "页 " + "共" + tostring(max_base + 1) + "页";
		   }

		   // 点菜信息
		   {
			   dish_sale *root = dish_sale_root;
			   el_sale[0].push_back(vbox(text("菜肴名称"), separator()));
			   el_sale[1].push_back(vbox(text("桌位号"), separator()));
			   el_sale[2].push_back(vbox(text("顾客编号"), separator()));
			   el_sale[3].push_back(vbox(text("顾客名称"), separator()));
			   el_sale[4].push_back(vbox(text("数量"), separator()));
			   el_sale[5].push_back(vbox(text("单价"), separator()));
			   el_sale[6].push_back(vbox(text("总价"), separator()));
			   int now = 0, flag = 1;
			   while(root->next) {
				   if(now / 8 == page) {
					   if(flag) {
						   el_sale[0].push_back(vbox(text(root->name)) | flex);
						   el_sale[1].push_back(vbox(text(root->idx)) | flex);
						   el_sale[2].push_back(vbox(text(root->idx_customer)) | flex);
						   el_sale[3].push_back(vbox(text(root->name_customer)) | flex);
						   el_sale[4].push_back(vbox(text(tostring(root->num))) | flex);
						   el_sale[5].push_back(vbox(text(tostring(root->price))) | flex);
						   el_sale[6].push_back(vbox(text(tostring(root->sum))) | flex);
						   flag = 0;
					   } else {
						   el_sale[0].push_back(vbox(separatorEmpty(), text(root->name)) | flex);
						   el_sale[1].push_back(vbox(separatorEmpty(), text(root->idx)) | flex);
						   el_sale[2].push_back(vbox(separatorEmpty(), text(root->idx_customer)) | flex);
						   el_sale[3].push_back(vbox(separatorEmpty(), text(root->name_customer)) | flex);
						   el_sale[4].push_back(vbox(separatorEmpty(), text(tostring(root->num))) | flex);
						   el_sale[5].push_back(vbox(separatorEmpty(), text(tostring(root->price))) | flex);
						   el_sale[6].push_back(vbox(separatorEmpty(), text(tostring(root->sum))) | flex);
					   }
				   }
				   root = root->next;
				   now++;
			   }
			   max_sale = max((now - 1) / 8, max_sale);
			   if(selected == 2)
				   test = "第" + tostring(page + 1) + "页 " + "共" + tostring(max_sale + 1) + "页";
		   }

		   // 买单信息
		   {
			   check_out *root = check_out_root;
			   el_pay[0].push_back(vbox(text("桌位号"), separator()));
			   el_pay[1].push_back(vbox(text("顾客编号"), separator()));
			   el_pay[2].push_back(vbox(text("消费金额"), separator()));
			   el_pay[3].push_back(vbox(text("点菜时间"), separator()));
			   el_pay[4].push_back(vbox(text("服务员名称"), separator()));
			   el_pay[5].push_back(vbox(text("是否结账"), separator()));
			   int now = 0, flag = 1;
			   while(root->next) {
				   if(now / 8 == page) {
					   if(flag) {
						   el_pay[0].push_back(vbox(text(root->idx)) | flex);
						   el_pay[1].push_back(vbox(text(root->idx_customer)) | flex);
						   el_pay[2].push_back(vbox(text(tostring(root->sum))) | flex);
						   el_pay[3].push_back(vbox(text(root->time)) | flex);
						   el_pay[4].push_back(vbox(text(root->name_waiter)) | flex);
						   el_pay[5].push_back(vbox(text(root->is)) | flex);
						   flag = 0;
					   } else {
						   el_pay[0].push_back(vbox(separatorEmpty(), text(root->idx)) | flex);
						   el_pay[1].push_back(vbox(separatorEmpty(), text(root->idx_customer)) | flex);
						   el_pay[2].push_back(vbox(separatorEmpty(), text(tostring(root->sum))) | flex);
						   el_pay[3].push_back(vbox(separatorEmpty(), text(root->time)) | flex);
						   el_pay[4].push_back(vbox(separatorEmpty(), text(root->name_waiter)) | flex);
						   el_pay[5].push_back(vbox(separatorEmpty(), text(root->is)) | flex);
					   }
				   }
				   root = root->next;
				   now++;
			   }
			   max_pay = max((now - 1) / 8, max_pay);
			   if(selected == 3)
				   test = "第" + tostring(page + 1) + "页 " + "共" + tostring(max_pay + 1) + "页";
		   }

		   if(selected == 1) {
			   return vbox({
			              hbox(choose_base->Render() | flex, choose_sale->Render() | flex, choose_pay->Render() | flex),
			              text("菜肴信息") | center,
			              separator(),
			              hbox({
				  vbox(move(el_dish[0])) | flex,
				  separator(),
				  vbox(move(el_dish[1])) | flex,
				  separator(),
				  vbox(move(el_dish[2])) | flex,
				  separator(),
				  vbox(move(el_dish[3])) | flex,
				  separator(),
				  vbox(move(el_dish[4])) | flex,
			              }),
			              separator(),
			              hbox(page_last->Render(), separatorEmpty(), quit->Render(), separatorEmpty(), page_next->Render()) | center,
			              text(test) | align_right,
			          })
			          | border | size(WIDTH, EQUAL, 80) | center;
		   } else if(selected == 2) {
			   return vbox({
			              hbox(choose_base->Render() | flex, choose_sale->Render() | flex, choose_pay->Render() | flex),
			              text("点菜信息") | center,

			              separator(),
			              hbox({
				  vbox(move(el_sale[0])) | flex,
				  separator(),
				  vbox(move(el_sale[1])) | flex,
				  separator(),
				  vbox(move(el_sale[2])) | flex,
				  separator(),
				  vbox(move(el_sale[3])) | flex,
				  separator(),
				  vbox(move(el_sale[4])) | flex,
				  separator(),
				  vbox(move(el_sale[5])) | flex,
				  separator(),
				  vbox(move(el_sale[6])) | flex,
			              }),
			              separator(),
			              hbox(page_last->Render(), separatorEmpty(), quit->Render(), separatorEmpty(), page_next->Render()) | center,
			              text(test) | align_right,
			          })
			          | border | size(WIDTH, EQUAL, 80) | center;
		   } else  {//if(selected == 3)
			   return vbox({
			              hbox(choose_base->Render() | flex, choose_sale->Render() | flex, choose_pay->Render() | flex),
			              text("买单信息") | center,
			              separator(),
			              hbox({
				  vbox(move(el_pay[0])) | flex,
				  separator(),
				  vbox(move(el_pay[1])) | flex,
				  separator(),
				  vbox(move(el_pay[2])) | flex,
				  separator(),
				  vbox(move(el_pay[3])) | flex,
				  separator(),
				  vbox(move(el_pay[4])) | flex,
				  separator(),
				  vbox(move(el_pay[5])) | flex,
			              }),
			              separator(),
			              hbox(page_last->Render(), separatorEmpty(), quit->Render(), separatorEmpty(), page_next->Render()) | center,
			              text(test) | align_right,
			          })
			          | border | size(WIDTH, EQUAL, 80) | center;
		   }
        
	   });
	screen.Loop(renderer);
}

// 查找菜肴信息
void check_base() {
	test	 = "";
	string test_page = "";
	int selected = 1, page = 0, maxx = 0;
	set< dish_base * > sum;    // 记录查询到的菜肴信息
	dish_base in;	           // 输入菜肴信息
	string temp;
	auto screen	 = ScreenInteractive::TerminalOutput();
	auto choose_base = Button("菜肴信息", [&] { selected = 1; });
	auto choose_sale = Button("点菜信息", [&] { screen.Exit(); selected = 2; });
	auto choose_pay	 = Button("买单信息", [&] {screen.Exit(); selected = 3; });
	auto page_last	 = Button("上一页", [&] {if (page > 0)page--; });
	auto page_next	 = Button("下一页", [&] {if (page < maxx)page++; });

	Component input[5] = {
		Input(&in.name, "菜肴名称"),
		Input(&in.type, "菜肴种类"),
		Input(&temp, "菜肴单价"),
		Input(&in.other, "备注"),
		Input(&in.ingredients, "菜肴配料"),
	};


	auto quit  = Button("退出", [&] {
		 selected = 0;
		 screen.Exit();
	 });
	auto enter = Button("查询", [&] {
		// 查询菜肴信息
		sum.clear();
		for(auto it = map_dish_base.begin(); it != map_dish_base.end(); it++) {
			// 查找菜肴名称
			if(!in.name.empty() && in.name.find(it->second->name) == string::npos && it->second->name.find(in.name) == string::npos)
				continue;

			// 查找菜肴种类
			if(!in.type.empty() && in.type.find(it->second->type) == string::npos && it->second->type.find(in.type) == string::npos)
				continue;

			// 查找单价
			if(!temp.empty() && temp != tostring(it->second->price))
				continue;

			// 查找备注
			if(!in.other.empty() && in.other.find(it->second->other) == string::npos && it->second->other.find(in.other) == string::npos)
				continue;

			// 查找菜肴配料
			if(!in.ingredients.empty() && in.ingredients.find(it->second->ingredients) == string::npos && it->second->ingredients.find(in.ingredients) == string::npos)
				continue;

			sum.insert(it->second);
		}
		test = "已找到" + tostring(sum.size()) + "个菜肴信息";
	});

	auto comp     = Container::Horizontal({
	        // 组件列表
	        input[0],
	        input[1],
	        input[2],
	        input[3],
	        input[4],
	        choose_base,
	        choose_sale,
	        choose_pay,
	        enter,
	        quit,
	        page_last,
	        page_next,
	    });
	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		Elements Elements[5];
		Elements[0].push_back(vbox(text("菜肴名称"), separatorEmpty(), input[0]->Render(), separator()));
		Elements[1].push_back(vbox(text("菜肴种类"), separatorEmpty(), input[1]->Render(), separator()));
		Elements[2].push_back(vbox(text("菜肴单价"), separatorEmpty(), input[2]->Render(), separator()));
		Elements[3].push_back(vbox(text("备注"), separatorEmpty(), input[3]->Render(), separator()));
		Elements[4].push_back(vbox(text("菜肴配料"), separatorEmpty(), input[4]->Render(), separator()));
		int now = 0, flag = 1;
		for(auto root : sum) {
			if(now / 7 == page) {
				if(flag) {
					Elements[0].push_back(vbox(text(root->name)) | flex);
					Elements[1].push_back(vbox(text(root->type)) | flex);
					Elements[2].push_back(vbox(text(tostring(root->price))) | flex);
					Elements[3].push_back(vbox(text(root->other)) | flex);
					Elements[4].push_back(vbox(text(root->ingredients)) | flex);
					flag = 0;
				} else {
					Elements[0].push_back(vbox(separatorEmpty(), text(root->name)) | flex);
					Elements[1].push_back(vbox(separatorEmpty(), text(root->type)) | flex);
					Elements[2].push_back(vbox(separatorEmpty(), text(tostring(root->price))) | flex);
					Elements[3].push_back(vbox(separatorEmpty(), text(root->other)) | flex);
					Elements[4].push_back(vbox(separatorEmpty(), text(root->ingredients)) | flex);
				}
			}
			now++;
		}
		maxx      = max((now - 1) / 7, maxx);
		test_page = "第" + tostring(page + 1) + "页 " + "共" + tostring(maxx + 1) + "页";

		return vbox({
		           hbox(choose_base->Render() | flex, choose_sale->Render() | flex, choose_pay->Render() | flex),
		           text("菜肴信息") | center,
		           text(test),
		           separator(),
		           hbox({
		               vbox(move(Elements[0])) | flex,
		               separator(),
		               vbox(move(Elements[1])) | flex,
		               separator(),
		               vbox(move(Elements[2])) | flex,
		               separator(),
		               vbox(move(Elements[3])) | flex,
		               separator(),
		               vbox(move(Elements[4])) | flex,
		           }),
		           separator(),
		           hbox(page_last->Render(), separatorEmpty(), enter->Render(), separatorEmpty(), quit->Render(),
			separatorEmpty(), page_next->Render())
		               | center,
		           text(test_page) | align_right,
		       })
		       | border | size(WIDTH, EQUAL, 80) | center;
	});
	screen.Loop(renderer);
	if(selected == 2) {
		check_sale();
	} else if(selected == 3) {
		check_check();
	}
}

// 查找点菜信息
void check_sale() {
	test	 = "";
	string test_page = "";
	int selected = 2, page = 0, maxx = 0;
	set< dish_sale * > sum;    // 记录查询到的点菜信息

	dish_sale in;	           // 输入点菜信息

	string temp[3];
	auto screen	 = ScreenInteractive::TerminalOutput();
	auto choose_base = Button("菜肴信息", [&] {screen.Exit(); selected = 1; });
	auto choose_sale = Button("点菜信息", [&] { selected = 2; });
	auto choose_pay	 = Button("买单信息", [&] {screen.Exit(); selected = 3; });
	auto page_last	 = Button("上一页", [&] {if (page > 0)page--; });
	auto page_next	 = Button("下一页", [&] {if (page < maxx)page++; });

	Component input[7] = {
		Input(&in.name, "菜肴名称"),
		Input(&in.idx, "桌位号"),
		Input(&in.idx_customer, "顾客编号"),
		Input(&in.name_customer, "顾客名称"),
		Input(&temp[0], "数量"),
		Input(&temp[1], "单价"),
		Input(&temp[2], "总价"),
	};
	auto quit     = Button("退出", [&] {
		    selected = 0;
		    screen.Exit();
	    });
	auto enter    = Button("查询", [&] {
		   // 查询点菜信息
		   sum.clear();
		   for(auto it = map_dish_sale.begin(); it != map_dish_sale.end(); it++) {
			   // 查找菜肴名称
			   if(!in.name.empty() && in.name.find(it->second->name) == string::npos && it->second->name.find(in.name) == string::npos)
				   continue;

			   // 查找桌位号
			   if(!in.idx.empty() && in.idx != it->second->idx)
				   continue;

			   // 查找顾客编号
			   if(!in.idx_customer.empty() && in.idx_customer != it->second->idx_customer)
				   continue;

			   // 查找顾客名称
			   if(!in.name_customer.empty() && in.name_customer.find(it->second->name_customer) == string::npos && it->second->name_customer.find(in.name_customer) == string::npos)
				   continue;

			   // 查找数量
			   if(!temp[0].empty() && temp[0] != tostring(it->second->num))
				   continue;

			   // 查找单价
			   if(!temp[1].empty() && temp[1] != tostring(it->second->price))
				   continue;

			   // 查找总价
			   if(!temp[2].empty() && temp[2] != tostring(it->second->sum))
				   continue;


			   sum.insert(it->second);
		   }
		   test = "已找到" + tostring(sum.size()) + "个点菜信息";
	   });
	auto comp     = Container::Horizontal({
	        // 组件列表
	        input[0],
	        input[1],
	        input[2],
	        input[3],
	        input[4],
	        input[5],
	        input[6],
	        choose_base,
	        choose_sale,
	        choose_pay,
	        enter,
	        quit,
	        page_last,
	        page_next,
	    });
	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		Elements elements[7];
		elements[0].push_back(vbox(text("菜肴名称"), separatorEmpty(), input[0]->Render(), separator()));
		elements[1].push_back(vbox(text("桌位号"), separatorEmpty(), input[1]->Render(), separator()));
		elements[2].push_back(vbox(text("顾客编号"), separatorEmpty(), input[2]->Render(), separator()));
		elements[3].push_back(vbox(text("顾客名称"), separatorEmpty(), input[3]->Render(), separator()));
		elements[4].push_back(vbox(text("数量"), separatorEmpty(), input[4]->Render(), separator()));
		elements[5].push_back(vbox(text("单价"), separatorEmpty(), input[5]->Render(), separator()));
		elements[6].push_back(vbox(text("总价"), separatorEmpty(), input[6]->Render(), separator()));
		int now = 0, flag = 1;
		for(auto root : sum) {
			if(now / 7 == page) {
				if(flag) {
					elements[0].push_back(vbox(text(root->name)) | flex);
					elements[1].push_back(vbox(text(root->idx)) | flex);
					elements[2].push_back(vbox(text(root->idx_customer)) | flex);
					elements[3].push_back(vbox(text(root->name_customer)) | flex);
					elements[4].push_back(vbox(text(tostring(root->num))) | flex);
					elements[5].push_back(vbox(text(tostring(root->price))) | flex);
					elements[6].push_back(vbox(text(tostring(root->sum))) | flex);
					flag = 0;
				} else {
					elements[0].push_back(vbox(separatorEmpty(), text(root->name)) | flex);
					elements[1].push_back(vbox(separatorEmpty(), text(root->idx)) | flex);
					elements[2].push_back(vbox(separatorEmpty(), text(root->idx_customer)) | flex);
					elements[3].push_back(vbox(separatorEmpty(), text(root->name_customer)) | flex);
					elements[4].push_back(vbox(separatorEmpty(), text(tostring(root->num))) | flex);
					elements[5].push_back(vbox(separatorEmpty(), text(tostring(root->price))) | flex);
					elements[6].push_back(vbox(separatorEmpty(), text(tostring(root->sum))) | flex);
				}
			}
			now++;
		}
		maxx      = max((now - 1) / 7, maxx);
		test_page = "第" + tostring(page + 1) + "页 " + "共" + tostring(maxx + 1) + "页";


		return vbox({
		           hbox(choose_base->Render() | flex, choose_sale->Render() | flex, choose_pay->Render() | flex),
		           text("点菜信息") | center,
		           text(test),
		           separator(),
		           hbox({
		               vbox(move(elements[0])) | flex,
		               separator(),
		               vbox(move(elements[1])) | flex,
		               separator(),
		               vbox(move(elements[2])) | flex,
		               separator(),
		               vbox(move(elements[3])) | flex,
		               separator(),
		               vbox(move(elements[4])) | flex,
		               separator(),
		               vbox(move(elements[5])) | flex,
		               separator(),
		               vbox(move(elements[6])) | flex,
		           }),
		           separator(),
		           hbox(page_last->Render(), separatorEmpty(), enter->Render(), separatorEmpty(), quit->Render(),
			separatorEmpty(), page_next->Render())
		               | center,
		           text(test_page) | align_right,
		       })
		       | border | size(WIDTH, EQUAL, 80) | center;
	});

	screen.Loop(renderer);
	if(selected == 1) {
		check_base();
	} else if(selected == 3) {
		check_check();
	}
}

// 查找买单信息
void check_check() {
	test	 = "";
	string test_page = "";
	int selected = 3, page = 0, maxx = 0;
	set< check_out * > sum;    // 记录查询到的买单信息
	check_out in;	           // 输入买单信息
	string temp;
	auto screen	 = ScreenInteractive::TerminalOutput();
	auto choose_base = Button("菜肴信息", [&] {screen.Exit(); selected = 1; });
	auto choose_sale = Button("点菜信息", [&] { screen.Exit(); selected = 2; });
	auto choose_pay	 = Button("买单信息", [&] { selected = 3; });
	auto page_last	 = Button("上一页", [&] {if (page > 0)page--; });
	auto page_next	 = Button("下一页", [&] {if (page < maxx)page++; });

	Component input[6] = {
		Input(&in.idx, "桌位号"),
		Input(&in.idx_customer, "顾客编号"),
		Input(&temp, "消费金额"),
		Input(&in.time, "点菜时间"),
		Input(&in.name_waiter, "服务员名称"),
		Input(&in.is, "是否结账"),
	};

	auto quit     = Button("退出", [&] {
		    selected = 0;
		    screen.Exit();
	    });
	auto enter    = Button("查询", [&] {
		   // 查询买单信息
		   sum.clear();
		   for(auto it = map_check_out.begin(); it != map_check_out.end(); it++) {
			   // 查找桌位号
			   if(!in.idx.empty() && in.idx != it->second->idx)
				   continue;

			   // 查找顾客编号
			   if(!in.idx_customer.empty() && in.idx_customer != it->second->idx_customer)
				   continue;

			   // 查找消费金额
			   if(!temp.empty() && temp != tostring(it->second->sum))
				   continue;

			   // 查找点菜时间
			   if(!in.time.empty() && in.time != it->second->time)
				   continue;

			   // 查找服务员名称
			   if(!in.name_waiter.empty() && in.name_waiter != it->second->name_waiter)
				   continue;

			   // 查找是否结账
			   if(!in.is.empty() && in.is != it->second->is)
				   continue;

			   sum.insert(it->second);
		   }
		   test = "已找到" + tostring(sum.size()) + "个买单信息";
	   });
	auto comp     = Container::Horizontal({
	        // 组件列表
	        input[0],
	        input[1],
	        input[2],
	        input[3],
	        input[4],
	        input[5],
	        choose_base,
	        choose_sale,
	        choose_pay,
	        enter,
	        quit,
	        page_last,
	        page_next,
	    });
	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		Elements elements[6];

		elements[0].push_back(vbox(text("桌位号"), separatorEmpty(), input[0]->Render(), separator()));
		elements[1].push_back(vbox(text("顾客编号"), separatorEmpty(), input[1]->Render(), separator()));
		elements[2].push_back(vbox(text("消费金额"), separatorEmpty(), input[2]->Render(), separator()));
		elements[3].push_back(vbox(text("点菜时间"), separatorEmpty(), input[3]->Render(), separator()));
		elements[4].push_back(vbox(text("服务员名称"), separatorEmpty(), input[4]->Render(), separator()));
		elements[5].push_back(vbox(text("是否结账"), separatorEmpty(), input[5]->Render(), separator()));
		int now = 0, flag = 1;
		for(auto root : sum) {
			if(now / 7 == page) {
				if(flag) {

					elements[0].push_back(vbox(text(root->idx)) | flex);
					elements[1].push_back(vbox(text(root->idx_customer)) | flex);
					elements[2].push_back(vbox(text(tostring(root->sum))) | flex);
					elements[3].push_back(vbox(text(root->time)) | flex);
					elements[4].push_back(vbox(text(root->name_waiter)) | flex);
					elements[5].push_back(vbox(text(root->is)) | flex);
					flag = 0;
				} else {
					elements[0].push_back(vbox(separatorEmpty(), text(root->idx)) | flex);
					elements[1].push_back(vbox(separatorEmpty(), text(root->idx_customer)) | flex);
					elements[2].push_back(vbox(separatorEmpty(), text(tostring(root->sum))) | flex);
					elements[3].push_back(vbox(separatorEmpty(), text(root->time)) | flex);
					elements[4].push_back(vbox(separatorEmpty(), text(root->name_waiter)) | flex);
					elements[5].push_back(vbox(separatorEmpty(), text(root->is)) | flex);
				}
			}
			now++;
		}
		maxx      = max((now - 1) / 7, maxx);
		test_page = "第" + tostring(page + 1) + "页 " + "共" + tostring(maxx + 1) + "页";

		return vbox({
		           hbox(choose_base->Render() | flex, choose_sale->Render() | flex, choose_pay->Render() | flex),
		           text("买单信息") | center,
		           text(test),
		           separator(),
		           hbox({
		               vbox(move(elements[0])) | flex,
		               separator(),
		               vbox(move(elements[1])) | flex,
		               separator(),
		               vbox(move(elements[2])) | flex,
		               separator(),
		               vbox(move(elements[3])) | flex,
		               separator(),
		               vbox(move(elements[4])) | flex,
		               separator(),
		               vbox(move(elements[5])) | flex,
		           }),
		           separator(),
		           hbox(page_last->Render(), separatorEmpty(), enter->Render(), separatorEmpty(), quit->Render(),
			separatorEmpty(), page_next->Render())
		               | center,
		           text(test_page) | align_right,
		       })
		       | border | size(WIDTH, EQUAL, 80) | center;

	});

	screen.Loop(renderer);
	if(selected == 1) {
		check_base();
	} else if(selected == 2) {
		check_sale();
	}
}

// 统计菜肴信息
void vount_base() {
	test	 = "";
	string test_page = "";
	int selected = 1, page = 0, maxx = 0;
	map< string, int > vount;    // 菜肴种类统计
	for(auto it = map_dish_base.begin(); it != map_dish_base.end(); it++) {
		vount[it->second->type]++;
	}
	auto screen	 = ScreenInteractive::TerminalOutput();
	auto choose_base = Button("菜肴统计", [&] { selected = 1; });
	auto choose_sale = Button("点菜统计", [&] {screen.Exit(); selected = 2; });
	auto choose_pay	 = Button("买单统计", [&] {screen.Exit(); selected = 3; });
	auto page_last	 = Button("上一页", [&] {if (page > 0)page--; });
	auto page_next	 = Button("下一页", [&] {if (page < maxx)page++; });

	auto quit = Button("退出", [&] {
		selected = 0;
		screen.Exit();
	});
	auto comp = Container::Horizontal({
	    // 组件列表
	    choose_base,
	    choose_sale,
	    choose_pay,
	    quit,
	    page_last,
	    page_next,
	});


	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		Elements elements[2];
		elements[0].push_back(vbox(text("菜肴种类"), separator()));
		elements[1].push_back(vbox(text("数量"), separator()));
		int now = 0, flag = 1;
		for(auto it = vount.begin(); it != vount.end(); it++) {
			if(now / 7 == page) {
				if(flag) {
					elements[0].push_back(vbox(text(it->first)) | flex);
					elements[1].push_back(vbox(text(tostring(it->second))) | flex);
					flag = 0;
				} else {
					elements[0].push_back(vbox(separatorEmpty(), text(it->first)) | flex);
					elements[1].push_back(vbox(separatorEmpty(), text(tostring(it->second))) | flex);
				}
			}
			now++;
		}
		maxx      = max((now - 1) / 7, maxx);
		test_page = "第" + tostring(page + 1) + "页 " + "共" + tostring(maxx + 1) + "页";

		return vbox({
		           hbox(choose_base->Render() | flex, choose_sale->Render() | flex, choose_pay->Render() | flex),
		           text("菜肴统计") | center,
		           text(test),
		           separator(),

		           hbox({
		               vbox(move(elements[0])) | flex,
		               separator(),
		               vbox(move(elements[1])) | flex,
		           }),
		           separator(),
		           hbox(page_last->Render(), separatorEmpty(), quit->Render(),
			separatorEmpty(), page_next->Render())
		               | center,
		           text(test_page) | align_right,
		       })
		       | border | size(WIDTH, EQUAL, 80) | center;

	});

	screen.Loop(renderer);
	if(selected == 2) {
		vount_sale();
	} else if(selected == 3) {
		vount_check();
	}
}

// 统计点菜信息
void vount_sale() {
	test	 = "";
	string test_page = "";
	int selected = 2, page = 0, maxx = 0;
	map< string, double > vount;	// 点菜信息统计
	for(auto it = map_dish_sale.begin(); it != map_dish_sale.end(); it++) {
		vount[it->second->name] += it->second->sum;
	}
	auto screen	 = ScreenInteractive::TerminalOutput();
	auto choose_base = Button("菜肴统计", [&] { screen.Exit(); selected = 1; });
	auto choose_sale = Button("点菜统计", [&] { selected = 2; });
	auto choose_pay	 = Button("买单统计", [&] {screen.Exit(); selected = 3; });
	auto page_last	 = Button("上一页", [&] {if (page > 0)page--; });
	auto page_next	 = Button("下一页", [&] {if (page < maxx)page++; });

	auto quit = Button("退出", [&] {
		selected = 0;
		screen.Exit();
	});

	auto comp     = Container::Horizontal({
	        // 组件列表
	        choose_sale,
	        choose_base,
	        choose_pay,
	        quit,
	        page_last,
	        page_next,
	    });
	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		Elements elements[2];

		elements[0].push_back(vbox(text("菜肴名称"), separator()));
		elements[1].push_back(vbox(text("消费金额"), separator()));
		int now = 0, flag = 1;
		for(auto it = vount.begin(); it != vount.end(); it++) {
			if(now / 7 == page) {
				if(flag) {
					elements[0].push_back(vbox(text(it->first)) | flex);
					elements[1].push_back(vbox(text(tostring(it->second))) | flex);
					flag = 0;
				} else {
					elements[0].push_back(vbox(separatorEmpty(), text(it->first)) | flex);
					elements[1].push_back(vbox(separatorEmpty(), text(tostring(it->second))) | flex);
				}
			}
			now++;
		}
		maxx      = max((now - 1) / 7, maxx);
		test_page = "第" + tostring(page + 1) + "页 " + "共" + tostring(maxx + 1) + "页";

		return vbox({
		           hbox(choose_base->Render() | flex, choose_sale->Render() | flex, choose_pay->Render() | flex),
		           text("点菜统计") | center,
		           text(test),
		           separator(),
		           hbox({
		               vbox(move(elements[0])) | flex,
		               separator(),
		               vbox(move(elements[1])) | flex,
		           }),
		           separator(),
		           hbox(page_last->Render(), separatorEmpty(), quit->Render(),
			separatorEmpty(), page_next->Render())
		               | center,
		           text(test_page) | align_right,
		       })
		       | border | size(WIDTH, EQUAL, 80) | center;

	});

	screen.Loop(renderer);
	if(selected == 1) {
		vount_base();
	} else if(selected == 3) {
		vount_check();
	}
}

// 统计买单信息
void vount_check() {
	test	 = "";
	string test_page = "";
	int selected = 3, page = 0, maxx = 0, choose = 1;
	map< string, double > vount[4];	   // 买单信息统计
	for(auto it = map_check_out.begin(); it != map_check_out.end(); it++) {
		vount[0][it->second->idx] += it->second->sum;
		vount[1][it->second->idx_customer] += it->second->sum;
		vount[2][it->second->time] += it->second->sum;
		vount[3][it->second->name_waiter] += it->second->sum;
	}
	auto screen	 = ScreenInteractive::TerminalOutput();
	auto choose_base = Button("菜肴统计", [&] {screen.Exit(); selected = 1; });
	auto choose_sale = Button("点菜统计", [&] { screen.Exit(); selected = 2; });
	auto choose_pay	 = Button("买单统计", [&] { selected = 3; });
	auto page_last	 = Button("上一页", [&] {if (page > 0)page--; });
	auto page_next	 = Button("下一页", [&] {if (page < maxx)page++; });

	Component button[4] = {
		Button("桌位号", [&] { choose	  = 1; }),
		Button("顾客编号", [&] { choose = 2; }),
		Button("点菜时间", [&] { choose = 3; }),
		Button("服务员名称", [&] { choose = 4; }),
	};

	auto quit     = Button("退出", [&] {
		    selected = 0;
		    screen.Exit();
	    });
	auto comp     = Container::Horizontal({
	        // 组件列表
	        choose_pay,
	        choose_base,
	        choose_sale,
	        button[0],
	        button[1],
	        button[2],
	        button[3],
	        quit,
	        page_last,
	        page_next,
	    });
	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		Elements elements[5];

		elements[0].push_back(vbox(button[0]->Render(), separator()));
		elements[1].push_back(vbox(button[1]->Render(), separator()));
		elements[2].push_back(vbox(button[2]->Render(), separator()));
		elements[3].push_back(vbox(button[3]->Render(), separator()));
		elements[4].push_back(vbox(separatorEmpty(), text("    消费金额"), separatorEmpty(), separator()));

		maxx    = 0;
		int now = 0, flag = 1;
		for(auto it = vount[choose - 1].begin(); it != vount[choose - 1].end(); it++) {
			if(now / 7 == page) {
				if(flag) {
					elements[choose - 1].push_back(vbox(text(it->first)) | flex);
					elements[4].push_back(vbox(text(tostring(it->second))) | flex);
					flag = 0;
				} else {
					elements[choose - 1].push_back(vbox(separatorEmpty(), text(it->first)) | flex);
					elements[4].push_back(vbox(separatorEmpty(), text(tostring(it->second))) | flex);
				}
			}
			now++;
		}
		maxx      = max((now - 1) / 7, maxx);
		test_page = "第" + tostring(page + 1) + "页 " + "共" + tostring(maxx + 1) + "页";

		return vbox({
		           hbox(choose_base->Render() | flex, choose_sale->Render() | flex, choose_pay->Render() | flex),
		           text("买单统计") | center,
		           text(test),
		           separator(),
		           hbox({
		               vbox(move(elements[0])) | flex,
		               separator(),
		               vbox(move(elements[1])) | flex,
		               separator(),
		               vbox(move(elements[2])) | flex,
		               separator(),
		               vbox(move(elements[3])) | flex,
		               separator(),
		               vbox(move(elements[4])) | flex,
		           }),
		           separator(),
		           hbox(page_last->Render(), separatorEmpty(), quit->Render(), separatorEmpty(), page_next->Render()) | center,
		           text(test_page) | align_right,
		       })
		       | border | size(WIDTH, EQUAL, 80) | center;

	});

	screen.Loop(renderer);
	if(selected == 1) {
		vount_base();
	} else if(selected == 2) {
		vount_sale();
	}
}

// 汇总报表
void summary() {

	int schedule	 = -1;
	test	 = "";
	string test_page = "";
	int page = 0, maxx = 0, choose = 1;

	// 统计菜肴交易信息
	map< pair< string, string >, pair< double, double > > vount;
	string time, name;
	for(auto it = map_check_out.begin(); it != map_check_out.end(); it++) {
		time = it->second->time;
		while(time.back() != '-') {
			time.pop_back();
		}
		time.pop_back();
		name	          = map_dish_sale[it->second->idx_sale]->name;
		pair< double, double > &temp = vount[{ name, time }];
		temp.first += map_dish_sale[it->second->idx_sale]->num;
		temp.second += it->second->sum;
		vount[{ name, time }] = temp;
	}

	auto screen = ScreenInteractive::TerminalOutput();

	auto page_last = Button("上一页", [&] {if (page > 0)page--; });
	auto page_next = Button("下一页", [&] {if (page < maxx)page++; });

	auto quit = Button("退出", [&] {
		screen.Exit();
	});
	auto comp = Container::Horizontal({
	    // 组件列表
	    quit,
	    page_last,
	    page_next,
	});

	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		// 等待进度条
		if(schedule != 6) {
			screen.PostEvent(Event::Custom);    // 抛出事件，让屏幕响应
			if(schedule != -1)
				_sleep(300);
			schedule++;
			return vbox({
			    separatorEmpty() | size(HEIGHT, EQUAL, 11),
			    border(gauge(schedule / 6.0)) | size(WIDTH, EQUAL, 40) | center,
			});
		}
		Elements elements[5];

		elements[0].push_back(vbox(text("菜肴名称"), separator()));
		elements[1].push_back(vbox(text("交易月份"), separator()));
		elements[2].push_back(vbox(text("菜肴单价"), separator()));
		elements[3].push_back(vbox(text("交易数量"), separator()));
		elements[4].push_back(vbox(text("收入"), separator()));

		maxx    = 0;
		int now = 0, flag = 1;
		for(auto it = vount.begin(); it != vount.end(); it++) {
			if(now / 7 == page) {
				if(flag) {
					elements[0].push_back(vbox(text(it->first.first)) | flex);
					elements[1].push_back(vbox(text(it->first.second)) | flex);
					elements[2].push_back(vbox(text(tostring(map_dish_base[it->first.first]->price))) | flex);
					elements[3].push_back(vbox(text(tostring(it->second.first))) | flex);
					elements[4].push_back(vbox(text(tostring(it->second.second))) | flex);
					flag = 0;
				} else {
					elements[0].push_back(vbox(separatorEmpty(), text(it->first.first)) | flex);
					elements[1].push_back(vbox(separatorEmpty(), text(it->first.second)) | flex);
					elements[2].push_back(vbox(separatorEmpty(), text(tostring(map_dish_base[it->first.first]->price))) | flex);
					elements[3].push_back(vbox(separatorEmpty(), text(tostring(it->second.first))) | flex);
					elements[4].push_back(vbox(separatorEmpty(), text(tostring(it->second.second))) | flex);
				}
			}
			now++;
		}
		maxx      = max((now - 1) / 7, maxx);
		test_page = "第" + tostring(page + 1) + "页 " + "共" + tostring(maxx + 1) + "页";
		return vbox({
		           text("汇总信息") | center,
		           text(test),
		           separator(),
		           hbox({
		               vbox(move(elements[0])) | flex,
		               separator(),
		               vbox(move(elements[1])) | flex,
		               separator(),
		               vbox(move(elements[2])) | flex,
		               separator(),
		               vbox(move(elements[3])) | flex,
		               separator(),
		               vbox(move(elements[4])) | flex,
		           }),
		           separator(),
		           hbox(page_last->Render(), separatorEmpty(), quit->Render(), separatorEmpty(), page_next->Render()) | center,
		           text(test_page) | align_right,
		       })
		       | border | size(WIDTH, EQUAL, 80) | center;
	});
	screen.Loop(renderer);
}

// 主菜单
void menu() {
	auto screen	         = ScreenInteractive::TerminalOutput();
	int selected	         = 0;
	vector< string > entries = {
		"录入菜肴基本信息",
		"顾客点菜",
		"顾客买单",
		"修改菜肴基本信息",
		"输出全部信息",
		"查找指定信息",
		"统计指定信息",
		"汇总报表",
	};
	auto enter = Button("进入", [&] {
		switch(selected) {
			case 0:
				in();
				break;
			case 1:
				order();
				break;
			case 2:
				pay();
				break;
			case 3:
				in_change_dish();
				break;
			case 4:
				print();
				break;
			case 5:
				check_base();
				break;
			case 6:
				vount_base();
				break;
			case 7:
				summary();
				break;
		}
	});
	auto quit  = Button("退出", screen.ExitLoopClosure());
	auto menu  = Menu(&entries, &selected);    // 构建菜单

	auto comp = Container::Horizontal({
	    // 组件列表
	    menu,
	    enter,
	    quit,
	});



	auto renderer = Renderer(comp, [&] {    // 渲染菜单
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		string s = tostring(sys.wYear) + "/" + tostring(sys.wMonth) + "/" + tostring(sys.wDay) + " " + tostring(sys.wHour)
		           + ":" + tostring(sys.wMinute) + ":" + tostring(sys.wSecond);
		return vbox({
		           text(s) | center | color(Color::GrayDark),
		           text("餐饮管理系统") | center,
		           separatorEmpty(),
		           menu->Render(),
		           hbox(enter->Render(), separatorEmpty(), quit->Render()) | center,
		       })
		       | border | size(WIDTH, EQUAL, 40) | center;
	});
	screen.Loop(renderer);
}

// 登陆界面
void menu_login() {
	auto screen	= ScreenInteractive::TerminalOutput();
	string account	= "";
	string password = "";

	InputOption password_option;
	password_option.password = true;

	Component button_login    = Button("登录", [&] {
		   if(account.empty() || password.empty()) {
			   test = "请输入账号密码";
		   } else if(map_account[account].empty()) {	// 账号信息不存在
			   test     = "账号不存在";
			   account  = "";
			   password = "";
		   } else if(map_account[account] == password) {    // 密码正确
			   test = "";
			   user = account;
			   loading();
			   screen.Exit();
			   menu();
		   } else {    // 密码错误
			   test     = "密码错误";
			   password = "";
		   }
	   });
	Component button_register = Button("注册", [&] { menu_register(); });      // 打开注册界面
	Component button_change   = Button("修改密码", [&] { menu_change(); });    // 打开修改密码界面
	Component input_account   = Input(&account, "请输入账号");

	Component input_password = Input(&password, "请输入密码", password_option);

	auto comp     = Container::Vertical({
	        input_account,
	        input_password,
	        button_login,
	        button_register,
	        button_change,
	    });
	auto renderer = Renderer(comp, [&] {    // 渲染comp
		return vbox({
		    separatorEmpty(),
		    separatorEmpty(),
		    separatorEmpty(),
		    separatorEmpty(),
		    separatorEmpty(),
		    separatorEmpty(),
		    vbox({
		        text("Hello " + account) | center,
		        separator(),
		        hbox(text("账号"), input_account->Render()),
		        hbox(text("密码"), input_password->Render()),
		        separator(),
		        hbox(button_login->Render(), separatorEmpty(), button_register->Render(), separatorEmpty(), button_change->Render()) | center,
		        text(test) | color(Color::Red),
		    }) | border
		        | size(WIDTH, EQUAL, 40) | center,
		});
	});

	screen.Loop(renderer);
}

// 注册界面
void menu_register() {
	test	= "";
	auto screen	= ScreenInteractive::TerminalOutput();
	string account	= "";
	string password = "";

	Component back	         = Button("返回", [&] {test = ""; screen.Exit(); });
	Component input_account  = Input(&account, "请输入账号");
	Component input_password = Input(&password, "请输入密码");

	Component button_register = Button("注册", [&] {    // 注册按钮
		if(account.empty() || password.empty()) {
			test = "请输入账号密码";
		} else if(account.find("\\") != account.npos || account.find("/") != account.npos
		          || account.find(":") != account.npos || account.find("*") != account.npos
		          || account.find("?") != account.npos || account.find("\"") != account.npos
		          || account.find("<") != account.npos || account.find(">") != account.npos
		          || account.find("|") != account.npos) {
			test    = "输入用户名非法，不能包含\\/:*?\"<>|";
			account = "";
		} else if(map_account[account].empty()) {    // 账号信息不存在
			test	     = "注册成功 请登录";
			map_account[account] = password;
			string gen	     = "md data\\" + account;
			system(gen.c_str());    // 创建文件夹

			gen = "type NUL > data\\" + account + "\\dish_base.dat";
			system(gen.c_str());    // 创建dish_base.dat
			gen = "type NUL > data\\" + account + "\\dish_sale.dat";
			system(gen.c_str());    // 创建dish_sale.dat
			gen = "type NUL > data\\" + account + "\\check-out.dat";
			system(gen.c_str());    // 创建check-out.dat

			save();
			screen.Exit();
		} else {    // 账号信息存在
			test    = "账号存在";
			account = "";
		}
	});
	auto comp	          = Container::Vertical({
	    input_account,
	    input_password,
	    button_register,
	    back,
	});
	auto renderer	          = Renderer(comp, [&] {    // 渲染comp
		            return vbox({
			separatorEmpty(),
			separatorEmpty(),
			separatorEmpty(),
			separatorEmpty(),
			separatorEmpty(),
			separatorEmpty(),
			vbox({
			    text("Hello " + account) | center,
			    separator(),
			    hbox(text("账号"), input_account->Render()),
			    hbox(text("密码"), input_password->Render()),
			    separator(),
			    hbox(button_register->Render(), separatorEmpty(), back->Render()) | center,
			    text(test) | color(Color::Red),
			}) | border
			    | size(WIDTH, EQUAL, 40) | center,
		            });
	            });

	screen.Loop(renderer);
}

// 修改密码界面
void menu_change() {
	test	= "";
	auto screen	= ScreenInteractive::TerminalOutput();
	string account	= "";
	string password = "";

	Component back	         = Button("返回", [&] {test = ""; screen.Exit(); });
	Component input_account  = Input(&account, "请输入账号");
	Component input_password = Input(&password, "请输入密码");

	Component button_register = Button("修改密码", [&] {    // 修改密码按钮
		if(account.empty() || password.empty()) {
			test = "请输入账号和要修改的密码";
		} else if(map_account[account].empty()) {    // 账号信息不存在
			test = "账号不存在";
		} else {		             // 账号信息存在
			map_account[account] = password;
			save();
			test = "密码修改成功";
			screen.Exit();
		}
	});

	auto comp     = Container::Vertical({
	        input_account,
	        input_password,
	        button_register,
	        back,
	    });
	auto renderer = Renderer(comp, [&] {    // 渲染comp
		return vbox({
		    separatorEmpty(),
		    separatorEmpty(),
		    separatorEmpty(),
		    separatorEmpty(),
		    separatorEmpty(),
		    separatorEmpty(),
		    vbox({
		        text("Hello " + account) | center,
		        separator(),
		        hbox(text("账号"), input_account->Render()),
		        hbox(text("密码"), input_password->Render()),
		        separator(),
		        hbox(button_register->Render(), separatorEmpty(), back->Render()) | center,
		        text(test) | color(Color::Red),
		    }) | border
		        | size(WIDTH, EQUAL, 40) | center,
		});
	});

	screen.Loop(renderer);
}

int main() {
	system("chcp 65001");    // 指定编码
	system("cls");
	loading();
	menu_login();
	dish_base_root->del();    // 释放内存
	dish_sale_root->del();
	check_out_root->del();
	return 0;
}