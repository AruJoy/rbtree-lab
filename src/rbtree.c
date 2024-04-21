#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t* n = (node_t*)calloc(1, sizeof(node_t));

  n->color = RBTREE_BLACK;
  n->left = NULL;
  n->right = NULL;
  n->key = 0;
  n->parent = NULL;

  p->nil = n;
  p->root = p->nil;
  return p;
}

void delete_rbtree(rbtree *t) {
  if(t->root == NULL){
    free(t->nil);
    free(t);
    return;
  } else if(t->root == t->nil){
    free(t->nil);
    free(t);
    return;
  }
  
  rbtree *lTree = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *l_nil = (rbtree *)calloc(1, sizeof(rbtree));
  l_nil->color = RBTREE_BLACK;
  l_nil->key = 0;
  lTree->nil = l_nil;
  lTree->root = t->root->left;

  rbtree *rTree = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *r_nil = (rbtree *)calloc(1, sizeof(rbtree));
  r_nil->color = RBTREE_BLACK;
  r_nil->key = 0;
  rTree->nil = r_nil;
  rTree->root = t->root->right;

  delete_rbtree(lTree);
  delete_rbtree(rTree);

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
    // 이미 키값이 존재하면, 예외처리
    if (parent_node->key == key){
      return t->root;
    }
    else if (parent_node->key < key){
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
        node_t* greate_grandpa_node = grandpa_node->parent;
        if(greate_grandpa_node->key < grandpa_node->key){
          greate_grandpa_node->right = parent_node;
          parent_node->parent = greate_grandpa_node;
        }
        else{
          greate_grandpa_node->left = parent_node;
          parent_node->parent = greate_grandpa_node;
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
        node_t* greate_grandpa_node = grandpa_node->parent;
        if(greate_grandpa_node->key < grandpa_node->key){
          greate_grandpa_node->right = cur_node;
          cur_node->parent = greate_grandpa_node;
        }
        else{
          greate_grandpa_node->left = cur_node;
          cur_node->parent = greate_grandpa_node;
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
  }



  // 할아버지 기준으로 오른쪽
if(parent_position == 1){
    // linear
    if(insert_position == 1){
      // 회전을 위한 할아버지 위치 확인후, 그 자리에 부모 삽입
      node_t* greate_grandpa_node = grandpa_node->parent;
      if(greate_grandpa_node < grandpa_node){
        greate_grandpa_node->right = parent_node;
        parent_node->parent = greate_grandpa_node;
      }
      else{
        greate_grandpa_node->left = parent_node;
        parent_node->parent = greate_grandpa_node;
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
      node_t* greate_grandpa_node = grandpa_node->parent;
      if(greate_grandpa_node < grandpa_node){
        greate_grandpa_node->right = cur_node;
        cur_node->parent = greate_grandpa_node;
      }
      else{
        greate_grandpa_node->left = cur_node;
        cur_node->parent = greate_grandpa_node;
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
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
 