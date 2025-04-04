# Buddy 記憶體分配系統

## 一、概述

Buddy系統是一種記憶體分配演算法，用於高效管理實體記憶體的分配和釋放。系統將記憶體劃分為大小為2的冪次方的塊，每個塊可以進一步分割成兩個相等的"buddy"塊，或在釋放時合併回更大的塊。

## 二、數據結構

### 1. `page_t` 結構體

```c
typedef struct page {
    struct page* next;
    int order;
} page_t;
```

- `next`: 指向free_list中的下一個頁面
- `order`: 表示此塊的大小(2^order個頁面)

### 2. 全局數組

```c
page_t *free_list[MAX_ORDER];  // 每個order的空閑塊鏈表
int free_array[MAX_PAGES];     // 標記每個頁面的狀態
```

- `free_list`: 每個order維護一個空閒頁面鏈表
- `free_array`: 記錄每個頁面的狀態：
  - `ALLOCATED (-1)`: 頁面已分配
  - `BELONGS_TO_LARGER (-2)`: 屬於更大的塊
  - `0 ~ MAX_ORDER-1`: 表示此頁面是某個order的起始頁面

## 三、主要函數實現

### 1. 初始化 `buddy_init()`

```c
void buddy_init() {
    // 初始化所有free_list為NULL
    // 初始化free_array所有元素為BELONGS_TO_LARGER
    // 創建第一個最大的free塊(同時在初始free_list[MAX_ORDER-1]時，需要指派free_list[MAX_ORDER-1]->next = NULL和 free_list[MAX_ORDER-1]->next->next = NULL，因為第一次進入allocate時會把free_list[MAX_ORDER-1]的next指針指向free_list[MAX_ORDER-1]->next->next)
}
```

- 初始化時建立一個最大的連續記憶體塊(order = MAX_ORDER-1)

### 2. 分配 `allocate()`

```c
void *allocate(unsigned int size) {
    // 計算所需order
    // 尋找合適的free，從所需order開始查找free_list，直到找到可用order
    while(current_order<MAX_ORDER && !page){
      if(free_list[current_order]!=NULL){         //check if the free list is not empty(already split the page)
          page=free_list[current_order];
          free_list[current_order]=page->next;    //remove the page from the free list[current_order] 
                                                  //ex: free_list[1]: [block@0] -> [block@2] -> NULL ===> free_list[1]: [block@2] -> NULL
          break;
      }
      current_order++;
    }
    // 如果order大於所需，則分割為更小的塊
    while(current_order>order){

        //計算split index
        unsigned int start_index = calculate_page_index(page);

        //取得split index address
        page_t* buddy = (page_t*)calculate_address(start_index + (1<<(current_order-1)));
        int buddy_index = start_index + (1<<(current_order-1));
        

        //update free_array
        free_array[start_index] = current_order-1;
        for(int i=start_index+1;i<buddy_index;i++){
            free_array[i] = BELONGS_TO_LARGER;
        }
        free_array[buddy_index] = current_order-1;
        for(int i=buddy_index+1;i< buddy_index+(1<<(current_order-1));i++){
            free_array[i] = BELONGS_TO_LARGER;
        }
        

        //update free_list
        free_list[current_order] = free_list[current_order]->next;
        buddy-> next = NULL;
        buddy-> order = current_order-1;
        page->order = current_order-1;
        free_list[current_order-1] = page;
        page->next = buddy;


        //print the split page
        uart_send_string("\n\r[+] Add page ");
        uart_send_int(buddy_index);
        uart_send_string(" to current_order ");
        uart_send_int(current_order);
        uart_send_string(".");

        current_order--;
    }
    //尋找第一個符合order的page index
    for(int i=0;i<MAX_PAGES;i++){
        if(free_array[i]==order){
            page_index=i;
            break;
        }
    }
    // 標記為已分配
    for(int i=0;i<(1<<order);i++){
        free_array[page_index+i]=ALLOCATED;
    }
    // 返回指針
}
```

分配過程：
1. 計算分配大小所需的order
2. 從所需order開始查找free_list，直到找到可用塊
3. 如果找到的塊current_order大於需要的order，則分割為更小的塊
    - 計算split index，用於計算buddy的index
    - 用split index 去計算address
    - 更新 free_array
    - 更新 free_list
    - current_order--
4. 標記塊為已分配並返回

### 3. 分割 (在allocate中實現)

分割過程：
1. 將大小為2^k的塊分割為兩個2^(k-1)的塊
2. 更新free_array狀態標記
3. 將兩個小塊加入free_list[k-1]

### 4. 釋放 `free()`

```c
void free(void *ptr) {
    // 計算頁面索引
    int index = calculate_page_index(page_ptr);
    page_t *page = (page_t *)page_ptr;
    
    // 檢查索引有效性
    if (free_array[index] != ALLOCATED) {
        uart_send_string("\n\rError: Trying to free unallocated memory\n\r");
        return;
    }
    // 根據指針轉換為page_t*去的到原本的page所屬的order
    int order = page->order;
    // 標記為空閒，這裡透過原先的order去算出占用的頁面數，並且把這些頁面標記為BELONGS_TO_LARGER，在把index的order設為order
    for(int i=index;i<index+(1<<order);i++){
        free_array[i]=BELONGS_TO_LARGER;
    }
    free_array[index]=order;
    // 嘗試與buddy合併，直到達到最大order，如果無法合併則跳出
    while (order < MAX_ORDER) {
        // 計算buddy的index，算法為index ^ (1<<order)，因為buddy的index是order的index 的位元反轉
        int buddy_index = index ^ (1<<order);
        // 檢查buddy是否也空閒，若是則合併
        if (buddy_index >= MAX_PAGES || free_array[buddy_index] != order) {
            break;
        }
        // 移除buddy
        page_t* prev = NULL;
        page_t* curr = free_list[order];
        // 在free_list中找到buddy，並且從free_list中移除
        while (curr && calculate_page_index((void*)curr) != buddy_index) {
            prev = curr;
            curr = curr->next;
        }
        // 如果buddy不存在，則跳出
        if (!curr) break;
        // 如果buddy是第一個node，則把free_list[order]指向curr的next
        if (prev) {
            prev->next = curr->next;
        } else {
            free_list[order] = curr->next;
        }
        // 合併找較小的index
        if (buddy_index < index) {
            index = buddy_index;
        }
        // 更新order
        order++;
        free_array[index] = order;
        // 標記buddy為belongs to a larger block
        for (int i = 0; i < (1 << (order - 1)); i++) {
            free_array[buddy_index + i] = BELONGS_TO_LARGER;
        }
        // 打印合併log
      }
      // 將合併後的塊加入對應的free_list，只在外面處理是因為最終只會有一個合併的塊加入free_list
      page_t* block = (page_t*)calculate_address(index);
      block->next = free_list[order];
      block->order = order;
      free_list[order] = block;
}
```

釋放過程：
1. 將塊標記為空閒
2. 進入while迴圈，
   - 檢查其buddy是否也空閒，若是則合併
   - 重複合併過程直到無法繼續合併或達到最大order
3. 將合併後的塊加入對應的free_list

### 5. 合併 (在free中實現)

合併過程：
1. 使用XOR操作快速找到buddy索引：`buddy_index = index ^ (1<<order)`
2. 檢查buddy是否空閒且order相同
3. 從free_list中移除buddy
4. 合併為更大的塊，更新order和free_array

## 四、複雜度分析

### 時間複雜度

- `allocate()`: O(MAX_ORDER + 2^order)
  - 尋找合適塊：O(MAX_ORDER)
  - 分割操作：O(MAX_ORDER)
  - 更新free_array：O(2^order)

- `free()`: O(MAX_ORDER + n + 2^order)
  - 合併操作：O(MAX_ORDER)
  - 在free_list搜尋：O(n)，n是該order的頁面數
  - 更新free_array：O(2^order)

### 空間複雜度

- `free_list`: O(MAX_ORDER)
- `free_array`: O(MAX_PAGES) = O(2^MAX_ORDER)
- 總體：O(2^MAX_ORDER)

## 五、優化方向

1. **位元圖優化**: 使用位元圖代替整數數組來標記頁面狀態
2. **平衡樹**: 對free_list使用平衡樹結構加速搜尋
3. **多層級管理**: 對不同大小的請求使用不同的策略
4. **預分配**: 常用大小的塊可以預先分配以減少分割操作

## 六、實現特點

1. 使用XOR操作快速找到buddy，減少計算開銷
2. 使用鏈表管理空閒塊，實現簡單
3. 通過order記錄塊大小，方便分割和合併操作
4. 使用free_array跟踪每個頁面狀態，便於合併判斷
5. 詳細的調試輸出幫助理解系統運行狀態

## 七、使用示例

### 基本使用

```c
// 初始化buddy系統
buddy_init();

// 分配4000字節記憶體
void* ptr1 = allocate(4000);  // 返回order 0的塊

// 分配8000字節記憶體
void* ptr2 = allocate(8000);  // 返回order 1的塊

// 釋放記憶體
free(ptr1);
free(ptr2);
```

### 詳細案例分析

假設我們有一個MAX_ORDER=4的buddy系統，初始狀態下有一個2^3=8頁的空閒塊。

#### 初始狀態

```
free_list[3] = [0-7]
free_list[0-2] = []
free_array = [3, -2, -2, -2, -2, -2, -2, -2]  // 頁面0是order=3的起始頁面
```

視覺化表示：
```
[ 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 ]  記憶體頁面
[        一個order=3的塊        ]  大小為8頁
```

#### 步驟1: 分配4000字節（約1頁）

```c
void* ptr1 = allocate(4000);  // 需要order=0的塊
```

1. 計算所需order = 0（單頁）
2. 沒有找到order=0的空閒塊，向上查找到order=3的塊
3. 分割過程：
   - 將order=3的塊分割為兩個order=2的塊
   - 將order=2的塊分割為兩個order=1的塊
   - 將order=1的塊分割為兩個order=0的塊
4. 分配一個order=0的塊，頁面索引0

分割後狀態：
```
free_list[0] = [1]
free_list[1] = [2-3]
free_list[2] = [4-7]
free_list[3] = []
free_array = [-1, 0, 1, -2, 2, -2, -2, -2]  // 頁面0已分配，頁面1是order=0，page index 2是order=1
```

視覺化表示：
```
[ 0* | 1 | 2  3 | 4  5  6  7 ]  記憶體頁面 (*表示已分配)
[ X  | 0 |   1  |      2     ]  塊的order
```

#### 步驟2: 分配8000字節（約2頁）

```c
void* ptr2 = allocate(8000);  // 需要order=1的塊
```

1. 計算所需order = 1（兩頁）
2. 找到一個order=1的空閒塊（頁面2-3）
3. 直接分配，無需分割
4. 分配一個order=1的塊，頁面索引2

分配後狀態：
```
free_list[0] = [1]
free_list[1] = []
free_list[2] = [4-7]
free_list[3] = []
free_array = [-1, 0, -1, -1, 2, -2, -2, -2]  // 頁面2-3已分配
```

視覺化表示：
```
[ 0* | 1 | 2* 3* | 4  5  6  7 ]  記憶體頁面
[ X  | 0 |   X   |     2      ]  塊的order
```

#### 步驟3: 釋放ptr1（頁面0）

```c
free(ptr1);  // 釋放頁面0
```

1. 標記頁面0為空閒，order=0
2. 檢查buddy（頁面1）是否空閒
3. 合併頁面0和頁面1為一個order=1的塊
4. 檢查新塊的buddy（頁面2-3）是否空閒
5. 不是空閒的，無法繼續合併
6. 將合併後的塊加入free_list[1]

釋放後狀態：
```
free_list[0] = []
free_list[1] = [0-1]
free_list[2] = [4-7]
free_list[3] = []
free_array = [1, -2, -1, -1, 2, -2, -2, -2]  // 頁面0-1是一個order=1的塊
```

視覺化表示：
```
[ 0  1 | 2* 3* | 4  5  6  7 ]  記憶體頁面
[   1   |   X   |     2     ]  塊的order
```

#### 步驟4: 釋放ptr2（頁面2-3）

```c
free(ptr2);  // 釋放頁面2-3
```

1. 標記頁面2-3為空閒，order=1
2. 檢查buddy（頁面0-1）是否空閒
3. 合併頁面0-1和頁面2-3為一個order=2的塊
4. 檢查新塊的buddy（頁面4-7）是否空閒
5. 是空閒的，繼續合併
6. 合併為一個order=3的塊
7. 將合併後的塊加入free_list[3]

釋放後狀態：
```
free_list[0] = []
free_list[1] = []
free_list[2] = []
free_list[3] = [0-7]
free_array = [3, -2, -2, -2, -2, -2, -2, -2]  // 回到初始狀態
```

視覺化表示：
```
[ 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 ]  記憶體頁面
[        一個order=3的塊        ]  大小為8頁
```

### 實際操作日誌示例

以下是一個實際運行的日誌示例：

```
=========[allocate size] 4000=========
[+] Add page 4 to current_order 3.
[+] Add page 2 to current_order 2.
[+] Add page 1 to current_order 1.
[after allocate][-1,0,1,-2,2,-2,-2,-2,]
free_list[0] : 0x10000000 -> 0x10001000 -> NULL.
free_list[1] : 0x10002000 -> NULL.
free_list[2] : 0x10004000 -> NULL.
free_list[3] : NULL.
[Page] Allocate 0x10000000 at order 0, page index 0
=========[allocate size] 8000=========
[after allocate][-1,0,-1,-1,2,-2,-2,-2,]
free_list[0] : 0x10001000 -> NULL.
free_list[1] : NULL.
free_list[2] : 0x10004000 -> NULL.
free_list[3] : NULL.
[Page] Allocate 0x10002000 at order 1, page index 2
=========[free] 0x10000000=========
[Merge] Merged page 0 with buddy 1 to form page 0 to order 1.
[after allocate][1,-2,-1,-1,2,-2,-2,-2,]
free_list[0] : NULL.
free_list[1] : 0x10000000 -> NULL.
free_list[2] : 0x10004000 -> NULL.
free_list[3] : NULL.
[-] Remove page index 0 from order 1.
=========[free] 0x10002000=========
[Merge] Merged page 0 with buddy 0 to form page 0 to order 2.
[Merge] Merged page 0 with buddy 4 to form page 0 to order 3.
[after allocate][3,-2,1,-2,-2,-2,-2,-2,]
free_list[0] : NULL.
free_list[1] : NULL.
free_list[2] : NULL.
free_list[3] : 0x10000000 -> NULL.
[-] Remove page index 0 from order 3.
```

這個日誌顯示了分配一個大小為4000字節和8000字節的記憶體塊後釋放它的過程，可以看到系統如何正確地分割和合併頁面。
