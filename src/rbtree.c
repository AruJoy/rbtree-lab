#include "rbtree.h"

#include <stdlib.h>

#include <stdio.h>

//=============================내부 활용 함수============================//
void rb_transplant(rbtree *t, node_t *u, node_t *v){
    if(u->parent == t->nil)
    {
      t->root = v;
    }
    else if(u == u->parent->left)
    {
        u->parent->left = v;
    }
    else
        u->parent->right = v;
    v->parent = u->parent;
    return;
}

void left_rotate(rbtree* t, node_t* x){
  node_t *y;
  y = x->right;
  x->right = y->left;
  if(y->left != t->nil){
    y->left->parent = x;
  }
  y->parent = x->parent;
  if(x->parent == t->nil){
    t->root = y;
  }else if(x == x->parent->left){
    x->parent->left = y;
  }else{
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
  return;
}

void right_rotate(rbtree* t, node_t *x){
  node_t* y;
  y = x->left;
  x->left = y->right;
  if(y->right != t->nil){
    y->right->parent = x;
  }
  y->parent = x->parent;
  if(x->parent == t->nil){
    t->root = y;
  }else if(x == x->parent->right){
    x->parent->right = y;
  }else{
    x->parent->left = y;
  }
  y->right = x;
  x->parent = y;
  return;
}

void rb_delete_fixup(rbtree* t, node_t* x){
  node_t* sibling;
  while ((x != t->root) && (x->color == RBTREE_BLACK)){
    if(x == x->parent->left){
      sibling = x->parent->right;
      if(sibling->color == RBTREE_RED){
        sibling->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        sibling = x->parent->right;
      }
      if(sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK){
        sibling->color = RBTREE_RED;
        x = x->parent;
      }else{
        if (sibling->right->color == RBTREE_BLACK){
          sibling->left->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          right_rotate(t, sibling);
          sibling = x->parent->right;
        }
        sibling->color = x -> parent -> color;
        x->parent->color = RBTREE_BLACK;
        sibling->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }else{
      sibling = x->parent->left;
      if (sibling->color == RBTREE_RED){
        sibling->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        sibling = x->parent->left;
      }
      if(sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK){
        sibling->color = RBTREE_RED;
        x = x->parent;
      }else{
        if(sibling->left->color == RBTREE_BLACK){
          sibling->right->color = RBTREE_BLACK;
          sibling->color = RBTREE_RED;
          left_rotate(t, sibling);
          sibling = x->parent->left;
        }
        sibling->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        sibling->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}
  
//=============================내부 활용 함수============================//

rbtree *new_rbtree(void) {
  // RB-tree / RB-tree에서 사용할 NIL node 생성
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t* n = (node_t*)calloc(1, sizeof(node_t));

  // nil노드 초기화 / pointer변수는 calloc했개 때문에 따로 초기화 해주지 않음
  n->color = RBTREE_BLACK;
  n->key = -2147483648;
  n->left = n;
  n->right = n;
  n->parent = n;
  
  // RB-tree 에소 사용할 NIL / root 설정 후 return
  p->nil = n;
  p->root = p->nil;
  return p;
}

void delete_rbtree(rbtree *t) {
  // 3. 탈툴조건: 선언된게 빈 서브트리일 경우,
  //            재귀를 위해 선언한 sub-tree 할당 취소 후 종료

  //빈 서브트리 조건: NIL노드의 부모는 NULL
  if(t->root->key == -2147483648){
    free(t->nil);
    free(t);
    return;
  }

  // 1-1. 왼쪽 sub-tree생성
  rbtree *lTree = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *l_nil = (node_t *)calloc(1, sizeof(node_t));
  l_nil->color = RBTREE_BLACK;
  l_nil->key = 0;
  lTree->nil = l_nil;
  lTree->root = t->root->left;
  // 1-2. 오른쪽 sub-tree생성
  rbtree *rTree = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *r_nil = (node_t *)calloc(1, sizeof(node_t));
  r_nil->color = RBTREE_BLACK;
  r_nil->key = 0;
  rTree->nil = r_nil;
  rTree->root = t->root->right;

  // 2. 왼쪽, 오른쫏 서브트리에 대해 재귀 호출
  delete_rbtree(lTree);
  delete_rbtree(rTree);

  // 4 leaf노드 부터 각 단계마다 서브트리의 root노드, 서브트리, nil 노드 삭제 후 return
  free(t->root);
  free(t->nil);
  free(t);
  return;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 삽입노드 선언
  node_t* cur_node = (node_t*)malloc(sizeof(node_t));
  // 삽입노드 초기화
  cur_node->key = key;
  cur_node->color = RBTREE_RED;
  cur_node->parent = t->nil;
  cur_node->left = t->nil;
  cur_node->right = t->nil;

  int insert_position = 0;    // 초기: 0, left: -1, right: 1
  int parent_position = 0;    // 초기: 0, left: -1, right: 1

  node_t* parent_node = t->root;
  node_t* great_grandpa_node;
  node_t* grandpa_node;
  node_t* ancle_node;

  // case1: 삽입위치가 루트노드면 루트에 삽입
  if(t->root == t->nil){
    t->root = cur_node;
    cur_node->color = RBTREE_BLACK;
    return t->root;
  }
  
  // 아닐경우, 삽입위치 탐색
  while(parent_node != t->nil){
      if (parent_node->key <= key){
      // 다음 위치가 nil이라면, break
      if(parent_node->right == t->nil){
        break;
      }
      // 아니면, 이동
      parent_node = parent_node->right;
    }else{
      // 다음 위치가 nil이라면, break
      if(parent_node->left == t->nil){
        break;
      }
      // 아니면, 이동
      parent_node = parent_node->left;
    }
  }

  // 삽입
  if (parent_node->key < cur_node->key){
    parent_node-> right = cur_node;
    cur_node->parent = parent_node;
  }
  else{
    parent_node-> left = cur_node;
    cur_node->parent = parent_node;
  }

//=====================case:1에 대한 상위 노드 체크 순회=====================//
  while(cur_node != t->root){
    // 현재위치의 부모노드와 삽입방향 확인
    parent_node = cur_node->parent;
    //상위 노드 순회중 부모노드가 root라면, 부모 black
    if (parent_node == t->root){
      parent_node->color = RBTREE_BLACK;
      return t->root;
    }
    // 부모기준, 현재위치 확인
    if (parent_node->key < cur_node->key){
      insert_position = 1;
    }
    else{
      insert_position = -1;
    }
    // 부모노드가 black이라면, 성공후 종료.
    if(parent_node->color == RBTREE_BLACK){
      return t->root;
    }
    // 부모노드 위치 확인
    grandpa_node = parent_node->parent;
    if (grandpa_node->key < parent_node->key){
      parent_position = 1;
    }
    else{
      parent_position = -1;
    }
    // 삼촌도 확인
    if(parent_position == -1){
      ancle_node = grandpa_node->right;
    }else{
      ancle_node = grandpa_node->left;
    }
    // 삼촌이 red라면, 색 지정 후 상위노드 순회.
    if (ancle_node->color == RBTREE_RED){
      grandpa_node->color = RBTREE_RED;
      parent_node->color = RBTREE_BLACK;
      ancle_node->color = RBTREE_BLACK;
      cur_node = grandpa_node;
      continue;
    }
    // 할아버지 기준으로 왼쪽
    if(parent_position == -1){
      // linear
      if(insert_position == -1){
        
        // 회전을 위한 할아버지 위치 확인후, 그 자리에 부모 삽입
        if(grandpa_node == t->root){
          t->root = parent_node;
          parent_node->parent = t->nil;
        }
        else{
          great_grandpa_node = grandpa_node->parent;
          if(great_grandpa_node->key < grandpa_node->key){
            great_grandpa_node->right = parent_node;
            parent_node->parent = great_grandpa_node;
          }
          else{
            great_grandpa_node->left = parent_node;
            parent_node->parent = great_grandpa_node;
          }
        }
        // 부모노드의 오른쪽애 힐아버지 삽입하기 전, 기존의 부모 오른쪽 임시 저장
        node_t* temp = parent_node->right;
        // 자리 정리
        parent_node->right = grandpa_node;
        grandpa_node->parent = parent_node;
        grandpa_node->left = temp;
        temp->parent = grandpa_node;
        // 색상 정리
        parent_node->color= RBTREE_BLACK;
        grandpa_node->color = RBTREE_RED;
        return t->root;
      }
      // triangle
      if(insert_position == 1){
        // 작은 회전 전, 현재노드 왼쪽 임시 저장
        node_t* temp = cur_node->left;
        // 작은회전 자리정리
        grandpa_node->left = cur_node;
        cur_node->parent = grandpa_node;
        cur_node->left = parent_node;
        parent_node->parent = cur_node;
        parent_node->right = temp;
        temp->parent = parent_node;

        // 큰회전을 위한 할아버지 위치 확인후, 그 자리에 현재노드 삽입
        if(grandpa_node == t->root){
          t->root = cur_node;
          cur_node->parent = t->nil;
        }
        else{
          great_grandpa_node = grandpa_node->parent;
          if(great_grandpa_node->key < grandpa_node->key){
            great_grandpa_node->right = cur_node;
            cur_node->parent = great_grandpa_node;
          }
          else{
            great_grandpa_node->left = cur_node;
            cur_node->parent = great_grandpa_node;
          }
        }
        
        // 현재노드 오른쪽에 할아버지 노드 삽입하기 전, 현재노드 오른쪽 임시 저장
        temp = cur_node->right;
        // 자리정리
        cur_node->right = grandpa_node;
        grandpa_node->parent = cur_node;
        grandpa_node->left = temp;
        temp->parent = grandpa_node;
        // 색상 정리
        cur_node->color = RBTREE_BLACK;
        parent_node->color = RBTREE_RED;
        grandpa_node->color = RBTREE_RED;
        return t->root;
      }
    }


  // 할아버지 기준으로 오른쪽
    if(parent_position == 1){
      // linear
      if(insert_position == 1){

        // 회전을 위한 할아버지 위치 확인후, 그 자리에 부모 삽입
        if(grandpa_node == t->root){
          t->root = parent_node;
          parent_node->parent = t->nil;
        }
        else{
          great_grandpa_node = grandpa_node->parent;
          if(great_grandpa_node->key < grandpa_node->key){
            great_grandpa_node->right = parent_node;
            parent_node->parent = great_grandpa_node;
          }
          else{
            great_grandpa_node->left = parent_node;
            parent_node->parent = great_grandpa_node;
          }
        }

        // 부모노드의 왼쪽애 할아버지 삽입하기 전, 기존의 부모 왼쪽 임시 저장
        node_t* temp = parent_node->left;
        // 자리 정리
        parent_node->left = grandpa_node;
        grandpa_node->parent = parent_node;
        grandpa_node->right = temp;
        temp->parent = grandpa_node;

        // 색상 정리
        parent_node->color = RBTREE_BLACK;
        grandpa_node->color = RBTREE_RED;
        return t->root;
      }

      // triangle
      if(insert_position == -1){
        // 작은 회전 전, 현재노드 오른쪽 임시 저장
        node_t* temp = cur_node->right;
        // 작은회전 자리정리
        grandpa_node->right = cur_node;
        cur_node->parent = grandpa_node;
        cur_node->right = parent_node;
        parent_node->parent = cur_node;
        parent_node->left = temp;
        temp->parent = parent_node;

        // 큰회전을 위한 할아버지 위치 확인후, 그 자리에 현재노드 삽입
        if(grandpa_node == t->root){
          t->root = cur_node;
          cur_node->parent = t->nil;
        }
        else{
          great_grandpa_node = grandpa_node->parent;
          if(great_grandpa_node->key < grandpa_node->key){
            great_grandpa_node->right = cur_node;
            cur_node->parent = great_grandpa_node;
          }
          else{
            great_grandpa_node->left = cur_node;
            cur_node->parent = great_grandpa_node;
          }
        }

        // 현재노드 왼쪽에 할아버지 노드 삽입하기 전, 현재노드 왼쪽 임시 저장
        temp = cur_node->left;
        // 자리정리
        cur_node->left = grandpa_node;
        grandpa_node->parent = cur_node;
        grandpa_node->right = temp;
        temp->parent = grandpa_node;

        // 색상 정리
        cur_node->color = RBTREE_BLACK;
        parent_node->color = RBTREE_RED;
        grandpa_node->color = RBTREE_RED;
        return t->root;
      }
    }
  }
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t* curNode = t->root;
  while (curNode != t->nil && curNode->key != key){
    if(curNode->key < key){
      curNode = curNode->right;
    }else{
      curNode = curNode->left;
    }
  }
  if(curNode == t->nil){
    return NULL;
  }
  return curNode;
}

node_t *rbtree_min(const rbtree *t) {
  if(t->root == t->nil){
    return NULL;
  }
  node_t *cur_node = t->root;
  while(cur_node->left != t->nil){
    cur_node = cur_node->left;
  }
  return cur_node;
}

node_t *rbtree_max(const rbtree *t) {
  if(t->root == t->nil){
    return NULL;
  }
  node_t *cur_node = t->root;
  while(cur_node->right != t->nil){
    cur_node = cur_node->right;
  }
  return cur_node;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t* deleted_node = p;
  color_t deleted_color = deleted_node->color;
  node_t* deleted_position;

  if(p->left == t->nil){
    deleted_position = p->right;
    rb_transplant(t, p, p->right);
  }else if(p->right == t->nil){
    deleted_position = p->left;
    rb_transplant(t, p, p->left);
  }else{
    node_t *cur_node = p->right;
    while(cur_node->left != t->nil){
      cur_node = cur_node->left;
    }
    deleted_node = cur_node;
    deleted_color = deleted_node->color;
    deleted_position = deleted_node->right;
    if (deleted_node -> parent == p){
      deleted_position->parent = deleted_node;
    }else{
      rb_transplant(t, deleted_node, deleted_node->right);
      deleted_node->right = p->right;
      deleted_node->right->parent = deleted_node;
    }
    rb_transplant(t, p, deleted_node);
    deleted_node->left = p->left;
    deleted_node->left->parent = deleted_node;
    deleted_node->color = p->color;
  }
  if(deleted_color == RBTREE_BLACK){
    rb_delete_fixup(t, deleted_position);
  }
  free(p);
  return 0;
}
int inorder_rbtree(node_t *root, key_t *res,const rbtree *t, int i){
    if (root == t -> nil){
      return i;
    }

    i = inorder_rbtree(root->left, res, t, i);
    res[i] = root->key;
    i += 1;
    i = inorder_rbtree(root->right, res, t, i);
    return i;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n){
    if (t->root == t->nil){
        return -1;
    }
    inorder_rbtree(t->root, arr, t, 0);
    return 0;
}


// // 트리를 출력하는 함수
// void print_rbtree(rbtree *t, node_t *node, int space)
// {
//   if (node == t->nil)
//     return;

//   space += 10;
//   print_rbtree(t, node->right, space);

//   printf("\n");
//   for (int i = 10; i < space; i++)
//     printf(" ");
//   printf("%d(%s)\n", node->key, node->color == RBTREE_RED ? "R" : "B");

//   print_rbtree(t, node->left, space);
// }

// int main() {
//     // 새로운 레드-블랙 트리 생성
//     rbtree *t = new_rbtree();

//     // 삽입할 노드 키 배열
//     int keys[] = {45, 32, 67, 12, 89, 24, 5, 78, 56, 11, 45, 23, 9, 100, 76, 54, 32, 18, 82, 97};


//     // 트리에 노드 삽입
//     for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
//         rbtree_insert(t, keys[i]);
//     }
//     print_rbtree(t, t->root, 0);
//     // 삭제할 노드 키 배열
//     int delete_keys[] = {45, 32, 67, 12, 89, 24, 5, 78, 56, 11, 45, 23, 9, 100, 76, 54, 32, 18, 82, 97};

//     // 트리에서 노드 삭제
//     for (int i = 0; i < sizeof(delete_keys) / sizeof(delete_keys[0]); i++) {
//         node_t *node = rbtree_find(t, delete_keys[i]);
//         if (node != NULL) {
//             rbtree_erase(t, node);
//             printf("\n\nAfter deleting node with key %d:\n", delete_keys[i]);
//             print_rbtree(t, t->root, 0);
//         } else {
//             printf("\nNode with key %d not found!\n", delete_keys[i]);
//         }
//     }

//     // 트리 메모리 해제
//     delete_rbtree(t);

//     return 0;
// }

// // 실패 함수

// int rbtree_erase(rbtree *t, node_t *p) {
//   // deleted_node = 실제 삭제 노드
//   node_t *deleted_node = p;
//   // deleted_position : 실제 삭제 연산이 일어난 위치
//   node_t *deleted_position = NULL;
//   // 삭제될 노드 색상 초기값: p->color
//   color_t deleted_color = deleted_node->color;
//   // 삽입 reansplant / rebalancing를 위한 지역변수
//   node_t *parent = NULL;
//   // rebalancing때 사용하기 위한 노드
//   node_t *sibling = NULL;

//   node_t* temp;
//   // 왼쪽 노드가 nil 일 경우, 삭제후 부모에 오른쪽 자식 연결
//   if(p->left == t->nil){
//     deleted_node = p->right;
//     //삭제노드가 root일 경우, root에 연결
//     if(p == t->root){
//       t->root = p->right;
//       p->right->parent = t->nil;
//     }else{
//       parent = p->parent;
//       if(p == p->parent->right){
//         parent->right = p->right;
//         p->right->parent = parent;
//       }
//       else{
//         parent->left = p->right;
//         p->right->parent = parent;
//       }
//     }
//   }
//   // 오른쪽 노드가 nil 일 경우, 삭제후 부모에 왼쪽 자식 연결
//   else if(p->right == t->nil){
//     deleted_node = p->left;
//     //삭제노드가 root일 경우, root에 연결
//     if(p == t->root){
//       t->root = p->left;
//       p->left->parent = t->nil;
//     }else{
//       parent = p->parent;
//       if(p == p->parent->right){
//         parent->right = p->left;
//         p->left->parent = parent;
//       }
//       else{
//         parent->left = p->left;
//         p->left->parent = parent;
//       }
//     }
//   }
//   else{
//     // 양쪽노드가 둘 다 존재하는 경우 -> successor 찾아서 대체
//     node_t *cur_node = p->right;
//     while(cur_node->left != t->nil){
//       cur_node = cur_node->left;
//     }
//     // 삭제될 노드 저장
//     deleted_position = cur_node;
//     // 삭제될 노드의 색상 저장
//     deleted_color = deleted_position->color;
//     // 삭제된 노드의 위치 저장
//     deleted_node = deleted_position->right;
//     //  deleted node의 부모가 delete_target 이라면, 오른쪽에 위치할것이다.
//     if(deleted_position-> parent == p){
//       deleted_node->parent = deleted_position;
//     }else{
//       // delete 노드의 우측 트리를 delete노드에 붙여 보존
//       parent = cur_node->parent;
//       // 아니라면, 왼쪽에 위치해있을것이다.
//       parent->left = cur_node->right;
//       cur_node->right->parent = parent;
//     }
//     if (p -> parent == t -> nil){
//         t -> root = deleted_position;
//     }
//     else if (p == p -> parent -> left){
//         p -> parent -> left = deleted_position;
//     }
//     else{
//         p -> parent -> right = deleted_position;
//     deleted_position -> parent = p -> parent;
//     }
//     deleted_position->left = p->left;
//     deleted_position->left->parent = deleted_position;
//     deleted_position->color = p->color;
//     }

//   //삭제 노드 색상이
//   if(deleted_color == RBTREE_BLACK){
//     //실제 삭제 위치가 순회중 재조정 위치가 root이거나, 조정 위치 색이 RED라면, 순회 종료.
//     while(deleted_node != t->root && deleted_node->color == RBTREE_BLACK){
//       //삭제 위치가 왼쪽 인 경우.
//       if(deleted_node == deleted_node->parent->left){
//         //오른쪽에 형제가 있음.
//         sibling = deleted_node->parent->right;
//         //만약 형제가 RED인경우, case.1 -> 형제와 부모의 색을 바꿔준 후, 삭제 위치 쪽으로 rotation.
//         if(sibling->color == RBTREE_RED){
//           sibling->color = RBTREE_BLACK;
//           deleted_node->parent->color = RBTREE_RED;
//           parent = deleted_node->parent;
//           if(parent == t->root){
//             t->root = sibling;
//             sibling->parent = t->nil;
//           }else if(parent == parent->parent->left){
//             parent->parent->left = sibling;
//             sibling->parent = parent->parent;
//           }else if(parent == parent->parent->right){
//             parent->parent->right = sibling;
//             sibling->parent = parent->parent;
//           }
//           temp = sibling->left;

//           sibling->left = parent;
//           parent->parent = sibling;
//           parent->right = temp;
//           temp->parent = parent;

//           sibling = deleted_node->parent->right;
//         }
//         // 만약 sibling이 red였다면, sibling의 왼쪽은 색이 RED로 변한 parent 일것이다.
//         // 따라서 sibling의 자식들이 black인걸로 case.1 판별가능
//         if(sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK){
//           sibling->color = RBTREE_RED;
//           deleted_node = deleted_node->parent;
//         // case.3 조카의 position이 triangle일때.
//         // 1. 조카와 sibling의 색을 교환 한 후, sibling 기준으로 작은 회전(linear가 되는 방향)
//         // 2. 이후, case4로 처리.
//         }else{
//           if(sibling->right->color == RBTREE_BLACK){
//             // 조카와 sibling 색 변경
//             sibling->left->color = RBTREE_BLACK;
//             sibling->color = RBTREE_RED;

//             // 오른 쪽 회전
//             parent = sibling->parent;
//             temp = sibling->left->right;
//             parent->right = sibling->left;
//             sibling->parent = parent;
//             sibling->left->right = sibling;
//             sibling->parent = sibling->left;
//             sibling->left = temp;
//             temp->parent = sibling;

//             sibling = sibling->parent->right;
//           }
//           //case4 -> 조카와 parent 의 색을 black으로 변경
//           //         sibling을 부모의 노드 계승
//           //         식제된 위치쪽으로 회전
//           sibling->color = deleted_node->parent->color;
//           deleted_node->parent->color = RBTREE_BLACK;
//           sibling->right->color = RBTREE_BLACK;

//           parent = deleted_node->parent;
//           if(parent == t->root){
//             t->root = sibling;
//             sibling->parent = t->root;
//           }else if(parent->parent->left == parent){
//             parent->parent->left = sibling;
//             sibling->parent = parent->parent;
//           }else{
//             parent->parent->right = sibling;
//             sibling->parent = parent->parent;
//           }
//           temp = sibling->left;
//           sibling->left = parent;
//           parent->parent = sibling;
//           parent->right = temp;
//           temp->parent = parent;
//           deleted_node = t->root;
//           }
//       }
//     //삭제 위치가 오른쪽 인 경우.
//       if(deleted_node == deleted_node->parent->right){
//         //왼쪽에 형제가 있음.
//         sibling = deleted_node->parent->left;
//         //만약 형제가 RED인경우, case.1 -> 형재와 부모의 색을 바꿔준 후, 삭제 위치 쪽으로 rotation.
//         if(sibling->color == RBTREE_RED){
//           sibling->color = RBTREE_BLACK;
//           deleted_node->parent->color = RBTREE_RED;
//           parent = deleted_node->parent;
//           if(parent == t->root){
//             t->root = sibling;
//             sibling->parent = t->nil;
//           }else if(parent == parent->parent->left){
//             parent->parent->left = sibling;
//             sibling->parent = parent->parent;
//           }else if(parent == parent->parent->right){
//             parent->parent->right = sibling;
//             sibling->parent = parent->parent;
//           }
//           temp = sibling->right;

//           sibling->right = parent;
//           parent->parent = sibling;
//           parent->left = temp;
//           temp->parent = parent;
//           sibling = deleted_node->parent->left;
//         }
//         // 만약 sibling이 red였다면, sibling의 오른쪽은 색이 RED로 변한 parent 일것이다.
//         // 따라서 sibling의 자식들이 black인걸로 case.1 판별가능
//         if(sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK){
//           sibling->color = RBTREE_RED;
//           deleted_node = deleted_node->parent;
//         // case.3 조카의 position이 triangle일때.
//         // 1. 조카와 sibling의 색을 교환 한 후, sibling 기준으로 작은 회전(linear가 되는 방향)
//         // 2. 이후, case4로 처리.
//         }else{
//           if(sibling->left->color == RBTREE_BLACK){
//           // 조카와 sibling 색 변경
//           sibling->right->color = RBTREE_BLACK;
//           sibling->color = RBTREE_RED;

//           // 오른 쪽 회전
//           parent = sibling->parent;
//           temp = sibling->right->left;
//           parent->left = sibling->right;
//           sibling->parent = parent;
//           sibling->right->left = sibling;
//           sibling->parent = sibling->right;
//           sibling->right = temp;
//           temp->parent = sibling;

//           sibling = sibling->parent->left;
//           }
//         //case4 -> 조카와 parent 의 색을 black으로 변경
//         //         sibling을 부모의 노드 계승
//         //         식제된 위치쪽으로 회전
//         sibling->color = deleted_node->parent->color;
//         sibling->parent->color = RBTREE_BLACK;
//         sibling->left->color = RBTREE_BLACK;

//         parent = deleted_node->parent;
//         if(parent == t->root){
//           t->root = sibling;
//           sibling->parent = t->root;
//         }else if(parent->parent->left == parent){
//           parent->parent->left = sibling;
//           sibling->parent = parent->parent;
//         }else{
//           parent->parent->right = sibling;
//           sibling->parent = parent->parent;
//         }
//         temp = sibling->right;
//         sibling->right = parent;
//         parent->parent = sibling;
//         parent->left = temp;
//         temp->parent = parent;
//         deleted_node = t->root;
//       }
//     }
//   }
//   }
//   free(p);
//   return 0;
// }