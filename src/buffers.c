
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "buffers.h"

//===================================================================================

alignment_data_t *alignment_data_new() {
  alignment_data_t *t = (alignment_data_t *)malloc(sizeof(alignment_data_t));
  
  return t;
}

void alignment_data_free(alignment_data_t *alignment_data) {
  if (alignment_data) { free(alignment_data); }
}


sa_alignment_t *sa_alignment_new(array_list_t *cals_list) {

  sa_alignment_t *sa_a = (sa_alignment_t *) malloc(sizeof(sa_alignment_t));

  sa_a->cals_list   = cals_list;
  sa_a->left_close  = 0;
  sa_a->right_close = 0;
  sa_a->c_left  = NULL;
  sa_a->c_right = NULL;
  sa_a->c_final = NULL;

  memset(sa_a->sp_middle, 0, 20);
  sa_a->num_sp = 0;
  sa_a->complete = 0;
  sa_a->reported = 0;

  sa_a->left_dsp_w2  = 0;
  sa_a->right_dsp_w2 = 0;

  return sa_a;

}


void sa_alignment_free(sa_alignment_t *sa_alignment) {
  free(sa_alignment);
}


batch_t *batch_new(bwt_server_input_t *bwt_input,
                   region_seeker_input_t *region_input,
                   cal_seeker_input_t *cal_input,
                   pair_server_input_t *pair_input,
		   preprocess_rna_input_t *preprocess_rna,
                   sw_server_input_t *sw_input,
                   batch_writer_input_t *writer_input,
		   int mapping_mode,
                   mapping_batch_t *mapping_batch) {

  batch_t *b = (batch_t *) calloc(1, sizeof(batch_t));
  b->bwt_input = bwt_input;
  b->region_input = region_input;
  b->cal_input = cal_input;
  b->pair_input = pair_input;
  b->sw_input = sw_input;
  b->writer_input = writer_input;
  b->mapping_batch = mapping_batch;
  b->mapping_mode = mapping_mode;
  b->preprocess_rna = preprocess_rna;

  return b;
}

void batch_free(batch_t *b) {
  if (b) free(b);
}


//====================================================================================

void region_batch_init(array_list_t **allocate_mapping_p, fastq_batch_t *unmapped_batch_p, region_batch_t *region_batch_p){
  region_batch_p->allocate_mapping_p = allocate_mapping_p;
  region_batch_p->unmapped_batch_p = unmapped_batch_p;
}

void region_batch_free(region_batch_t *region_batch_p){
  for(int i = 0; i < region_batch_p->unmapped_batch_p->num_reads; i++){
    array_list_free(region_batch_p->allocate_mapping_p[i], (void *)region_bwt_free);
  }
  free(region_batch_p->allocate_mapping_p);
  fastq_batch_free(region_batch_p->unmapped_batch_p);
  free(region_batch_p);
  
}

//====================================================================================

sw_batch_t* sw_batch_new(unsigned int num_reads, array_list_t **allocate_cals_p, 
			 fastq_read_t **allocate_reads_p) {
  sw_batch_t* sw_batch_p = (sw_batch_t *)malloc(sizeof(sw_batch_t));

  sw_batch_p->num_reads = num_reads;
  sw_batch_p->allocate_reads_p = allocate_reads_p;
  sw_batch_p->allocate_cals_p = allocate_cals_p;
  
  return sw_batch_p;
}

void sw_batch_free(sw_batch_t *sw_batch_p) {
  
  for(int i = 0; i < sw_batch_p->num_reads; i++){
    array_list_free(sw_batch_p->allocate_cals_p[i], (void *)cal_free);
    fastq_read_free(sw_batch_p->allocate_reads_p[i]);
  }

  free(sw_batch_p->allocate_cals_p);
  free(sw_batch_p->allocate_reads_p);
  free(sw_batch_p);
}
void sw_batch_init(unsigned int num_reads, array_list_t **allocate_cals_p, 
		   fastq_read_t **allocate_reads_p, sw_batch_t *sw_batch_p) {
  sw_batch_p->num_reads = num_reads;
  sw_batch_p->allocate_reads_p = allocate_reads_p;
  sw_batch_p->allocate_cals_p = allocate_cals_p;
}

//====================================================================================
//  write_batch functions
//====================================================================================

write_batch_t* write_batch_new(unsigned int allocate_size, unsigned char flag) {
  write_batch_t* write_batch_p = (write_batch_t*) calloc(1, sizeof(write_batch_t));
  
  write_batch_p->flag = flag;
  write_batch_p->size = 0;
  
  if(flag != MATCH_FLAG){
    write_batch_p->allocated_size = allocate_size;
    write_batch_p->buffer_p = (void *) calloc(allocate_size, sizeof(char));
  }else{
    write_batch_p->allocated_size = allocate_size/sizeof(alignment_t *);
    write_batch_p->buffer_p = (void *) calloc(write_batch_p->allocated_size, sizeof(alignment_t *));
  }
  return write_batch_p;
}

//------------------------------------------------------------------------------------

void write_batch_free(write_batch_t* write_batch_p) {
 if (write_batch_p == NULL) return;
 
 if (write_batch_p->buffer_p != NULL) free(write_batch_p->buffer_p);
 
 free(write_batch_p);
}

//====================================================================================

report_optarg_t *report_optarg_new(int all, int n_best, int n_hits, int only_paired, int best) {
  report_optarg_t *p = (report_optarg_t*) calloc(1, sizeof(report_optarg_t));

  p->all = all;
  p->n_best = n_best;
  p->n_hits = n_hits;
  p->only_paired = only_paired;
  p->best = best;

  return p;
}

//------------------------------------------------------------------------------------

void report_optarg_free(report_optarg_t *p) {
  if (p != NULL)
    free(p);
}

//====================================================================================

pair_mng_t *pair_mng_new(int pair_mode, size_t min_distance, 
			 size_t max_distance, int report_only_paired) {
  pair_mng_t *p = (pair_mng_t*) calloc(1, sizeof(pair_mng_t));

  p->pair_mode = pair_mode;
  p->min_distance = min_distance;
  p->max_distance = max_distance;
  p->report_only_paired = report_only_paired;

  return p;
}

//------------------------------------------------------------------------------------

void pair_mng_free(pair_mng_t *p) {
  if (p != NULL)
    free(p);
}

//====================================================================================

cal_batch_t* cal_batch_new(array_list_t **allocate_mapping, fastq_batch_t *unmapped_batch){
  cal_batch_t* cal_batch = (cal_batch_t *)malloc(sizeof(cal_batch_t));
  
  cal_batch->allocate_mapping = allocate_mapping;
  cal_batch->unmapped_batch = unmapped_batch;
  
  return cal_batch;
}

void cal_batch_free(cal_batch_t *cal_batch){
  for(int i = 0; i < cal_batch->unmapped_batch->num_reads; i++){
    array_list_free(cal_batch->allocate_mapping[i], (void *)region_bwt_free);
  }
  free(cal_batch->allocate_mapping);
  fastq_batch_free(cal_batch->unmapped_batch);
  free(cal_batch);
  
}

//====================================================================================

unsigned int pack_junction(unsigned int chromosome, unsigned int strand, 
			   size_t start, size_t end, size_t junction_id,
			   size_t num_reads, char *type, char* buffer_p){
  int len;
  char str[1024];
  char *p = buffer_p;
  char strand_char[2] = {'+', '-'};

  if (chromosome == 23) { sprintf(str, "%c%c", 'X', '\0'); }
  else if (chromosome == 24) { sprintf(str, "%c%c", 'Y', '\0'); }
  else if (chromosome == 25) { sprintf(str, "%s%c", "MT", '\0'); }
  else { sprintf(str, "%i%c", chromosome, '\0'); }
 
  len = strlen(str);
  memcpy(p, str, len);
  p += len;
  *p = '\t';
  p++;
  
  sprintf(str, "%lu", start);
  len = strlen(str);
  memcpy(p, str, len); 
  p += len;
  *p = '\t'; 
  p++;
  
  sprintf(str, "%lu", end);
  len = strlen(str);
  memcpy(p, str, len); 
  p += len;
  *p = '\t'; 
  p++;
  
  sprintf(str, "JUNCTION_%lu", junction_id);
  len = strlen(str);
  memcpy(p, str, len); 
  p += len;
  *p = '\t'; 
  p++;
  
  sprintf(str, "%lu", num_reads);
  len = strlen(str);
  memcpy(p, str, len); 
  p += len;
  *p = '\t'; 
  p++;
  
  *p = strand_char[strand]; 
  p++;
  *p = '\t'; 
  p++;
  
  len = strlen(type);
  memcpy(p, type, len);
  p += len;
  *p = '\n';
  p++;

  return (p - buffer_p);
}

//=====================================================================================
//=====================================================================================

mapping_batch_t *mapping_batch_new(array_list_t *fq_batch, pair_mng_t *pair_mng) {

  mapping_batch_t *p = (mapping_batch_t *) calloc(1, sizeof(mapping_batch_t));
  size_t num_reads = array_list_size(fq_batch);

  p->action = BWT_ACTION;
  p->num_targets = 0;
  p->num_extra_targets = 0;
  p->num_allocated_targets = num_reads;
  p->extra_stage_do = 0;

  if (!pair_mng) { 
    p->pair_mng = pair_mng_new(SINGLE_END_MODE, 0, 0, 0); 
  } else {
    p->pair_mng = pair_mng_new(pair_mng->pair_mode, pair_mng->min_distance, 
			       pair_mng->max_distance, pair_mng->report_only_paired); 
  }

  p->num_gaps = 0;
  p->num_sws = 0;
  p->num_ext_sws = 0;

  p->num_to_do = 0;
  p->fq_batch = fq_batch;
  p->targets = (size_t *) calloc(num_reads, sizeof(size_t));
  p->extra_targets = (size_t *) calloc(num_reads, sizeof(size_t));
  p->extra_stage_id = (unsigned char *) calloc(num_reads, sizeof(unsigned char));
  p->mapping_lists = (array_list_t **) calloc(num_reads, sizeof(array_list_t*));

  //for debug. TODO:delete
  p->bwt_mappings = (unsigned char *)calloc(num_reads, sizeof(unsigned char));

  for (size_t i = 0; i < num_reads; i++) {
    p->mapping_lists[i] = array_list_new(500, 
					 1.25f, 
					 COLLECTION_MODE_ASYNCHRONIZED); 
  }
    
  p->histogram_sw = (size_t *)calloc(1024, sizeof(size_t));

  // added by PP for bisulfite
  p->num_targets2 = 0;
  p->num_to_do2 = 0;
  p->targets2 = (size_t *) calloc(num_reads, sizeof(size_t));

  return p;
}

//------------------------------------------------------------------------------------

mapping_batch_t *mapping_batch_new_2(size_t num_reads, 
				     array_list_t *fq_batch, 
				     pair_mng_t *pair_mng) {

  mapping_batch_t *p = (mapping_batch_t *) calloc(1, sizeof(mapping_batch_t));

  p->action = BWT_ACTION;
  p->num_targets = 0;
  p->num_extra_targets = 0;
  p->num_allocated_targets = num_reads;
  p->extra_stage_do = 0;

  if (!pair_mng) { 
    p->pair_mng = pair_mng_new(SINGLE_END_MODE, 0, 0, 0); 
  } else {
    p->pair_mng = pair_mng_new(pair_mng->pair_mode, pair_mng->min_distance, 
			       pair_mng->max_distance, pair_mng->report_only_paired); 
  }

  p->num_gaps = 0;
  p->num_sws = 0;
  p->num_ext_sws = 0;

  p->num_to_do = 0;
  p->fq_batch = fq_batch;
  p->targets = (size_t *) calloc(num_reads, sizeof(size_t));
  p->extra_targets = (size_t *) calloc(num_reads, sizeof(size_t));
  p->extra_stage_id = (unsigned char *) calloc(num_reads, sizeof(unsigned char));
  p->mapping_lists = (array_list_t **) calloc(num_reads, sizeof(array_list_t*));

  /*for (size_t i = 0; i < num_reads; i++) {
    p->mapping_lists[i] = array_list_new(500, 
					 1.25f, 
					 COLLECTION_MODE_ASYNCHRONIZED); 
					 }*/

  //for debug. TODO:delete
  p->bwt_mappings = (unsigned char *)calloc(num_reads, sizeof(unsigned char));

  return p;
}

//------------------------------------------------------------------------------------

/*mapping_batch_t *mapping_batch_new_by_num(size_t num_reads, pair_mng_t *pair_mng) {

  mapping_batch_t *p = (mapping_batch_t *) calloc(1, sizeof(mapping_batch_t));

  p->action = BWT_ACTION;
  p->num_targets = 0;
  p->num_extra_targets = 0;
  p->num_allocated_targets = num_reads;
  p->extra_stage_do = 0;

  if (!pair_mng) { 
    p->pair_mng = pair_mng_new(SINGLE_END_MODE, 0, 0, 0); 
  } else {
    p->pair_mng = pair_mng_new(pair_mng->pair_mode, pair_mng->min_distance, 
			       pair_mng->max_distance, pair_mng->report_only_paired); 
  }

  p->num_to_do = 0;
  p->targets = (size_t *) calloc(num_reads, sizeof(size_t));
  p->extra_targets = (size_t *) calloc(num_reads, sizeof(size_t));
  p->extra_stage_id = (unsigned char *) calloc(num_reads, sizeof(unsigned char));
  p->mapping_lists = (array_list_t **) calloc(num_reads, sizeof(array_list_t*));
  p->old_mapping_lists = (array_list_t **) calloc(num_reads, sizeof(array_list_t*));

  for (size_t i = 0; i < num_reads; i++) {
    p->mapping_lists[i] = array_list_new(10, 
					 1.25f, 
					 COLLECTION_MODE_ASYNCHRONIZED);
  }
    
  return p;
}
*/

//------------------------------------------------------------------------------------

void mapping_batch_free(mapping_batch_t *p) {
  if (p == NULL) return;
  
  if (p->fq_batch) { array_list_free(p->fq_batch, (void *) fastq_read_free); }
  if (p->targets) { free(p->targets); }
  if (p->mapping_lists) { free(p->mapping_lists); }
  if (p->pair_mng) { free(p->pair_mng); }
  if (p->extra_stage_id) { free(p->extra_stage_id); }
  if (p->extra_targets) { free(p->extra_targets); }

  if (p->old_mapping_lists) { free(p->old_mapping_lists); }
  if (p->bwt_mappings) free(p->bwt_mappings);

  // added by PP
  if (p->CT_fq_batch) { array_list_free(p->CT_fq_batch, (void *) fastq_read_free); }
  if (p->CT_rev_fq_batch) { array_list_free(p->CT_rev_fq_batch, (void *) fastq_read_free); }
  if (p->GA_fq_batch) { array_list_free(p->GA_fq_batch, (void *) fastq_read_free); }
  if (p->GA_rev_fq_batch) { array_list_free(p->GA_rev_fq_batch, (void *) fastq_read_free); }
  if (p->mapping_lists2) { free(p->mapping_lists2); }
  if (p->targets2) { free(p->targets2); }
  
  free(p);
}

//------------------------------------------------------------------------------------
/*
rna_batch_t *rna_batch_new(array_list_t *fq_batch) {
  rna_batch_t *p = (rna_batch_t *) calloc(1, sizeof(rna_batch_t));

  size_t num_reads = array_list_size(fq_batch);

  p->action = BWT_ACTION;
  p->all_targets = 1;
  p->num_targets = 0;
  p->num_allocated_targets = num_reads;
  p->num_mapping_lists = num_reads;

  p->num_done = 0;
  p->num_to_do = 0;

  p->fq_batch = fq_batch;
  p->targets = (size_t *) calloc(num_reads, sizeof(size_t));
  p->mapping_lists = (array_list_t **) calloc(num_reads, sizeof(array_list_t*));
  for (size_t i = 0; i < num_reads; i++) {
    p->mapping_lists[i] = array_list_new(500, 
					 1.25f, 
					 COLLECTION_MODE_ASYNCHRONIZED); 
  }
    
  return p;
}

//------------------------------------------------------------------------------------

void rna_batch_free(rna_batch_t *p) {
  if (p == NULL) return;
  
  if (p->fq_batch != NULL) array_list_free(p->fq_batch, (void *)fastq_read_free);
  if (p->targets != NULL) free(p->targets);
  if (p->mapping_lists != NULL) { free(p->mapping_lists); }
  
  free(p);
}
*/

//------------------------------------------------------------------------------------

buffer_item_t *buffer_item_new() {
  return (buffer_item_t *)malloc(sizeof(buffer_item_t));
}


/*buffer_item_t *buffer_item_complete_new(fastq_read_t *fastq_read, array_list_t *items_list, void *aux_data) {
  buffer_item_t *buffer_item = buffer_item_new();
  buffer_item->read = fastq_read;
  buffer_item->items_list = array_list_new(array_list_size(items_list), 
					   1.25f, COLLECTION_MODE_ASYNCHRONIZED);
  
  for (int i = 0; i < array_list_size(items_list); i++) {
    void *item = array_list_get(i, items_list);
    array_list_insert(item, buffer_item->items_list);
  }

  buffer_item->aux_data = aux_data;

  return buffer_item;
  
}
*/

void alignment_aux_init(alignment_t* alignment, 
			alignment_aux_t *alignment_aux) {
  alignment_aux->seq_strand = alignment->seq_strand;
  alignment_aux->chromosome = alignment->chromosome; 
  alignment_aux->position = alignment->position;
  alignment_aux->num_cigar_operations = alignment->num_cigar_operations;
  alignment_aux->map_quality = alignment->map_quality;
  alignment_aux->optional_fields_length = alignment->optional_fields_length;
  alignment_aux->mapping_len = strlen(alignment->sequence);
  alignment_aux->cigar_len = strlen(alignment->cigar);
}


fastq_read_t *file_read_fastq_reads(size_t *num_items, FILE *fd) {

  size_t sizes_to_read[3], head_len, seq_len;
  int bytes;

  //start_timer(time_start);
  bytes = fread(sizes_to_read, sizeof(size_t), 3, fd);
  if (!bytes) { return NULL; }

  head_len   = sizes_to_read[0];
  seq_len    = sizes_to_read[1];
  *num_items = sizes_to_read[2];

  int tot_size = head_len + 2*seq_len;
  char *buffer = (char *)malloc((tot_size + 1) * sizeof(char));
  bytes = fread(buffer, sizeof(char), tot_size, fd);

  //stop_timer(time_start, time_end, time_read_fq);

  if (!bytes) {
    free(buffer);
    return NULL; 
  }

  //start_timer(time_start);
  char *id = (char *)malloc((head_len + 1)*sizeof(char));
  memcpy(id, buffer, head_len);
  id[head_len] = '\0';

  char *sequence = (char *)malloc((seq_len + 1)*sizeof(char));  
  memcpy(sequence, &buffer[head_len], seq_len);
  sequence[seq_len] = '\0';

  char *quality = (char *)malloc((seq_len + 1)*sizeof(char));  
  memcpy(quality, &buffer[head_len + seq_len], seq_len);
  quality[seq_len] = '\0';
  
  fastq_read_t *fq_read = fastq_read_new(id, sequence, quality);

  free(buffer);
  free(id);
  free(sequence);
  free(quality);

  //stop_timer(time_start, time_end, time_read_fq_process);

  return fq_read;

}

int file_read_cals(size_t num_items, array_list_t *list, 
		   fastq_read_t *fq_read, FILE *fd) {

  if (num_items == 0) { return 0; }

  int bytes;
  bwt_anchor_t bwt_anchors[num_items];
  bytes = fread(bwt_anchors, sizeof(bwt_anchor_t), num_items, fd);
  if (!bytes) { LOG_FATAL("Corrupt file\n"); }
  
  for (int i = 0; i < num_items; i++) {
    //printf("[%i:%lu-%lu]\n", bwt_anchors[i].chromosome, bwt_anchors[i].start, bwt_anchors[i].end);
    size_t seed_size = bwt_anchors[i].end - bwt_anchors[i].start;
    cal_t *cal;
    if (bwt_anchors[i].type == FORWARD_ANCHOR) {
      cal = (cal_t *)convert_bwt_anchor_to_CAL(&bwt_anchors[i], 0, seed_size);
    } else {
      cal = (cal_t *)convert_bwt_anchor_to_CAL(&bwt_anchors[i], fq_read->length - seed_size - 1, fq_read->length - 1);
    }
    //cal_print(cal);
    array_list_insert(cal, list); 
  }  
  
  return 0;

}

int file_read_meta_alignments(size_t num_items, array_list_t *list, 
                              fastq_read_t *fq_read, FILE *fd) {

  if (!num_items) { return 0; }

  simple_alignment_t simple_alignment[num_items];
  simple_alignment_t *simple_a;
  int bytes;

  bytes = fread(simple_alignment, sizeof(simple_alignment_t), num_items, fd);
  if (!bytes) { LOG_FATAL("Corrupt file\n"); }
  
  size_t cigar_tot_len = 0;
  for (int i = 0; i < num_items; i++) {
    simple_a = &simple_alignment[i];
    //printf("ITEM %i: (%i)[%i:%lu] [%i-%i]\n", i, simple_a->map_strand, simple_a->map_chromosome,
    //     simple_a->map_start, simple_a->gap_start, simple_a->gap_end);
    cigar_tot_len += simple_a->cigar_len;
  }
    
  char cigar_buffer[cigar_tot_len];
  bytes = fread(cigar_buffer, sizeof(char), cigar_tot_len, fd);
  if (!bytes) { LOG_FATAL("Corrupt file\n"); }

  char cigars_test[num_items][1024];
  size_t actual_read = 0;
  for (int i = 0; i < num_items; i++) {
    simple_a = &simple_alignment[i];
    memcpy(&cigars_test[i], &cigar_buffer[actual_read], simple_a->cigar_len);
    cigars_test[i][simple_a->cigar_len] = '\0';
    actual_read += simple_a->cigar_len;
    //printf("CIGAR %i: %s\n", i, cigars_test[i]);
    size_t map_len = fq_read->length - simple_a->gap_start - simple_a->gap_end;
    //printf("SEED := len_read:%i - gap_read:%i - gap_end:%i = %i, SEED-END = %i\n", fq_read->length, 
    //     simple_a->gap_start, 
    //     simple_a->gap_end, 
    //     map_len, simple_a->gap_start + map_len);
    seed_region_t *s_region = seed_region_new(simple_a->gap_start, 
                                              simple_a->gap_start + map_len - 1,
                                              simple_a->map_start, 
                                              simple_a->map_start + map_len - 1,
                                              0, 0, 0);
    
    //printf("Exit with seed [%i:%i]\n", s_region->read_start, s_region->read_end);
    
    linked_list_t *sr_list = linked_list_new(COLLECTION_MODE_ASYNCHRONIZED);
    //s_region->info = cigar_code_new_by_string(cigars_test[i]);
    linked_list_insert(s_region, sr_list);
    
    cal_t *cal = cal_new(simple_a->map_chromosome, 
                         simple_a->map_strand,
                         simple_a->map_start,
                         simple_a->map_start + map_len - 1,
                         1,
                         sr_list,
                         linked_list_new(COLLECTION_MODE_ASYNCHRONIZED));
    cigar_code_t *cc = cigar_code_new_by_string(cigars_test[i]);
    cc->distance = simple_a->map_distance;
    cal->info = cc;

    meta_alignment_t *meta_alignment = meta_alignment_new();    
    for (int m = 0; m < array_list_size(cc->ops); m++) {
      cigar_op_t *op = array_list_get(m, cc->ops);
      cigar_code_append_new_op(op->number, op->name, meta_alignment->cigar_code);
      //array_list_insert(cigar_op, ->ops);
    }
    meta_alignment->cigar_code->distance = simple_a->map_distance;

    array_list_insert(cal, meta_alignment->cals_list);
    array_list_insert(meta_alignment, list);

  }

  return 0;
}

int file_read_alignments(size_t num_items, array_list_t *list, 
			 fastq_read_t *fq_read, FILE *fd) {

  if (!num_items) { return 0; }

  //printf("Read file alignment...\n");

  alignment_aux_t alignments_aux[num_items];
  alignment_aux_t *alignment_a;
  int bytes;

  bytes = fread(alignments_aux, sizeof(alignment_aux_t), num_items, fd);
  if (!bytes) { LOG_FATAL("Corrupt file\n"); }

  size_t cigar_tot_len = 0;
  size_t of_tot_len = 0;
  for (int i = 0; i < num_items; i++) {
    alignment_a = &alignments_aux[i];

    //printf("CIGAR: %i + %i\n", cigar_tot_len,
    //	   alignment_a->cigar_len);
    cigar_tot_len += alignment_a->cigar_len;

    //printf("OF: %i + %i\n", of_tot_len,
    //	   alignment_a->optional_fields_length);
    of_tot_len    +=  alignment_a->optional_fields_length;

  }

  char cigar_buffer[cigar_tot_len];
  bytes = fread(cigar_buffer, sizeof(char), cigar_tot_len, fd);
  if (!bytes) { LOG_FATAL("Corrupt file\n"); }

  uint8_t of_buffer[of_tot_len];
  bytes = fread(of_buffer, sizeof(uint8_t), of_tot_len, fd);
  //if (!bytes) { LOG_FATAL("Corrupt file\n"); }

  char cigars_test[num_items][1024];
  size_t pos_cigar = 0, pos_of = 0;

  for (int i = 0; i < num_items; i++) {
    alignment_a = &alignments_aux[i];
    //printf("alignment_a->optional_fields_length = %i\n", alignment_a->optional_fields_length);

    memcpy(&cigars_test[i], &cigar_buffer[pos_cigar], alignment_a->cigar_len);
    cigars_test[i][alignment_a->cigar_len] = '\0';
    pos_cigar += alignment_a->cigar_len;
    
    char op;
    char op_value[1024];
    int c = 0;
    int hc_start = 0, hc_end = 0;

    //printf("CIGAR: %s\n", cigars_test[i]);

    for (int j = 0; j < alignment_a->cigar_len; j++) {
      op = cigars_test[i][j];
      if (op < 58) {
	op_value[c++] = op;
      } else {
	op_value[c] = '\0';
	if (op == 'H') {
	  hc_start = atoi(op_value);
	}
	break;
      }
    }

    c = 0;
    if (cigars_test[i][alignment_a->cigar_len - 1] == 'H') {
      for (int j = alignment_a->cigar_len - 2; j >= 0; j--) {
	op = cigars_test[i][j];
	//printf("Process op= %c\n", op);
	if (op < 58) {
	  op_value[c++] = op;
	} else {
	  op_value[c] = '\0';
	  int len = strlen(op_value);
	  char op_val_aux[len];
	  int pos = len - 1;
	  //printf("(%i) :: %s\n", len, op_value);
	  int t = 0;
	  for (t = 0; t < len; t++) {	    
	    op_val_aux[t] = op_value[pos - t];
	  } 
	  op_val_aux[t] = '\0';
	  //printf("(%i) :: %s\n", t, op_val_aux);
	  hc_end = atoi(op_val_aux);
	  break;
	}
      }
    }

    uint8_t *optional_fields = (uint8_t *)calloc(alignment_a->optional_fields_length, sizeof(uint8_t));

    memcpy(optional_fields, &of_buffer[pos_of], alignment_a->optional_fields_length);
    //optional_fields[alignment_a->optional_fields_length] = '0';
    pos_of += alignment_a->optional_fields_length;

    
    int header_len = strlen(fq_read->id);
    char header_id[header_len + 1];
    get_to_first_blank(fq_read->id, header_len, header_id);
    //char *header_match = (char *)malloc(sizeof(char)*header_len);
    //memcpy(header_match, header_id, header_len);

    int len_read = fq_read->length - (hc_start + hc_end);
    
    //printf("hc_start = %i, hc_end = %i, len_read = %i\n", 
    //	   hc_start, hc_end, len_read);
    char *quality = (char *) calloc (len_read + 1, sizeof(char));
    strncpy(quality, fq_read->quality + hc_start, len_read);
    
    char *query = (char *) calloc (len_read + 1, sizeof(char));
    strncpy(query, fq_read->sequence + hc_start, len_read);
 
    //Revisar rna_Server get_to_first_blank header copy
    alignment_t *alignment = alignment_new();
    alignment_init_single_end(strdup(header_id),
			      query,
			      quality,
			      alignment_a->seq_strand, 
			      alignment_a->chromosome, 
			      alignment_a->position,
			      strdup(cigars_test[i]),
			      alignment_a->num_cigar_operations,
			      alignment_a->map_quality, 
			      1, 
			      num_items < 1,
			      alignment_a->optional_fields_length,
			      optional_fields, 
			      alignment);
    array_list_insert(alignment, list);
  }  

  return 0;

}

void file_write_alignments(fastq_read_t *fq_read, array_list_t *items, FILE *fd) {
  //size_t head_size = strlen(fq_read->id);
  //size_t seq_size  = fq_read->length;

  size_t num_items = array_list_size(items);
  //printf("Num items %i\n", num_items);
  if (num_items <= 0) { return; }

  int tot_len_cigar = 0, tot_len_of = 0;
  //unsigned char type = ALIGNMENT_TYPE;

  //Write binary file 
  //[type][size head][size seq][num items][HEAD][SEQUENCE][QUALITY][ALIG 0][ALIG n][CIGAR STR][OF STR]
  //fwrite(type, sizeof(unsigned char), 1, fd);
  
  //size_t items_sizes[3] = {head_size, seq_size, num_items};
  //[size head][size seq][num items]
  //fwrite(items_sizes, sizeof(size_t), 3, fd);
  
  //[HEAD][SEQUENCE][QUALITY]
  size_t max_len_cigar = num_items*1024*2;
  char *buffer_cigar = (char *)malloc(sizeof(char)*max_len_cigar);

  size_t max_len_of = num_items*1024*2;
  uint8_t *buffer_of = (uint8_t *)malloc(sizeof(uint8_t)*max_len_of);
 
  //memcpy(buffer, fq_read->id, head_size);
  //memcpy(&buffer[head_size], fq_read->sequence, seq_size);
  //memcpy(&buffer[head_size + seq_size], fq_read->quality, seq_size);
  //fwrite(buffer, sizeof(char), total_size, fd);

  alignment_aux_t alignment_aux[num_items];
  alignment_aux_t *alignment_a;

  //printf("Num Items %d\n", num_items);

  memset(alignment_aux, 0, sizeof(alignment_aux_t)*num_items);  
  for (int i = 0; i < num_items; i++) {
    alignment_a = &alignment_aux[i];
    alignment_t *alignment = array_list_get(i, items);
    
    if (alignment->alig_data) {
      alignment->cigar = new_cigar_code_string(alignment->alig_data);
      array_list_clear(((cigar_code_t *)alignment->alig_data)->ops, (void *)cigar_op_free);
      cigar_code_free(alignment->alig_data);
    }

    alignment_aux_init(alignment, 
		       alignment_a);     

    int cigar_len = strlen(alignment->cigar);    
    memcpy(&buffer_cigar[tot_len_cigar], alignment->cigar, cigar_len);
    tot_len_cigar += cigar_len;
    
    if (tot_len_cigar >= max_len_cigar) { 
      max_len_cigar = max_len_cigar * 2;
      buffer_cigar = realloc(buffer_cigar, max_len_cigar); 
    }

    int of_len = alignment->optional_fields_length;
    //printf("ALig of len = %i\n", of_len);

    /*memcpy(&buffer_cigar[tot_len_cigar], alignment->optional_fields, of_len);
    tot_len_cigar += of_len;
    
    if (tot_len_cigar >= max_len_cigar) { 
      max_len_cigar = max_len_cigar * 2;
      buffer_cigar = realloc(buffer_cigar, max_len_cigar); 
    }
    */
    memcpy(&buffer_of[tot_len_of], alignment->optional_fields, of_len);
    tot_len_of += of_len;
    
    if (tot_len_of >= max_len_of) {
      max_len_of = max_len_of * 2;
      buffer_of = realloc(buffer_of, max_len_cigar); 
    }
        
  }

  fwrite(alignment_aux, sizeof(alignment_aux_t), num_items, fd);
  fwrite(buffer_cigar, sizeof(char), tot_len_cigar, fd);  
  fwrite(buffer_of, sizeof(uint8_t), tot_len_of, fd);  

  //free(buffer);
  free(buffer_cigar);
  free(buffer_of);

}

void file_write_meta_alignments(fastq_read_t *fq_read, array_list_t *items, FILE *fd) {
  //size_t head_size = strlen(fq_read->id);
  size_t seq_size  = fq_read->length;
  size_t num_items = array_list_size(items);
  if (!num_items) { return; }

  size_t max_len = num_items * 1024;
  char *cigar_buffer = (char *)calloc(max_len, sizeof(char));
  size_t tot_len = 0;

  simple_alignment_t simple_alignment[num_items];
  simple_alignment_t *simple_a;
  //unsigned char type = MENTA_TYPE;
  //fwrite(type, sizeof(unsigned char), 1, fd);

  memset(simple_alignment, 0, sizeof(simple_alignment_t)*num_items);
  //[type][size head][size seq][num items][HEAD][SEQUENCE][QUALITY][CAL 0][CAL n]
  for (int i = 0; i < num_items; i++) {
    meta_alignment_t *meta_alignment = array_list_get(i, items);
    cal_t *first_cal = array_list_get(0, meta_alignment->cals_list);
    cal_t *last_cal = array_list_get(meta_alignment->cals_list->size - 1, meta_alignment->cals_list);
    seed_region_t *first_seed = linked_list_get_first(first_cal->sr_list);
    seed_region_t *last_seed  = linked_list_get_last(last_cal->sr_list);
    cigar_code_t *cigar_code = meta_alignment->cigar_code;    
    char *cigar_str = new_cigar_code_string(cigar_code);
    int cigar_len = strlen(cigar_str);

    simple_a = &simple_alignment[i];

    if (meta_alignment->cigar_left !=  NULL) {
      //printf("LEFT CIGAR: %s\n", new_cigar_code_string(meta_alignment->cigar_left));
      simple_a->gap_start = 0;
    } else {
      simple_a->gap_start = first_seed->read_start;
    }

    if (meta_alignment->cigar_right !=  NULL) {
      //printf("RIGHT CIGAR: %s\n", new_cigar_code_string(meta_alignment->cigar_right));
      simple_a->gap_end = 0;
    } else {
      simple_a->gap_end = seq_size - last_seed->read_end - 1;
    }

    simple_a->map_strand = first_cal->strand;
    simple_a->map_chromosome = first_cal->chromosome_id;
    simple_a->map_start = first_cal->start;
    simple_a->map_distance = cigar_code->distance;
    simple_a->cigar_len = cigar_len;
    
    //printf(" [%i:%lu] INSERT CIGAR(%i): %s\n", simple_a->map_chromosome, 
    //	   simple_a->map_start, cigar_len, cigar_str);

    memcpy(&cigar_buffer[tot_len], cigar_str, cigar_len);
    tot_len += cigar_len;

    if (tot_len >= max_len) { 
      max_len = max_len * 2;
      cigar_buffer = realloc(cigar_buffer, max_len); 
    }

    free(cigar_str);
  }

  fwrite(simple_alignment, sizeof(simple_alignment_t), num_items, fd);
  fwrite(cigar_buffer, sizeof(char), tot_len, fd);  

  free(cigar_buffer);
}

void file_write_cals(fastq_read_t *fq_read, array_list_t *items, FILE *fd) {
  //size_t head_size = strlen(fq_read->id);
  //size_t seq_size  = fq_read->length;
  size_t num_items = array_list_size(items);
  if (!num_items) { return; }
  //Write binary file 
  //[type][size head][size seq][num items][HEAD][SEQUENCE][QUALITY][CAL 0][CAL n]
  //size_t items_sizes[3] = {head_size, seq_size, num_items};

  //[size head][size seq][num items]
  //fwrite(items_sizes, sizeof(size_t), 3, fd);
  
  //[HEAD][SEQUENCE][QUALITY]
  //size_t total_size = head_size + 2*seq_size;
  //char *buffer = (char *)malloc(sizeof(char)*total_size);
  
  //memcpy(buffer, fq_read->id, head_size);
  //memcpy(&buffer[head_size], fq_read->sequence, seq_size);
  //memcpy(&buffer[head_size + seq_size], fq_read->quality, seq_size);
  //fwrite(buffer, sizeof(char), total_size, fd);
  
  bwt_anchor_t bwt_anchor[num_items];
  memset(bwt_anchor, 0, sizeof(bwt_anchor_t)*num_items);

  for (int i = 0; i < num_items; i++) {
    cal_t *cal = array_list_get(i, items);
    //cal_print(cal);
    bwt_anchor[i].strand     = cal->strand;
    bwt_anchor[i].chromosome = cal->chromosome_id - 1;
    bwt_anchor[i].start      = cal->start;
    bwt_anchor[i].end        = cal->end;//cal->start + (cal->end - cal->start);
    seed_region_t *seed = linked_list_get_first(cal->sr_list);
    if (seed->read_start == 0) {
      bwt_anchor[i].type = FORWARD_ANCHOR;
    } else {
      bwt_anchor[i].type = BACKWARD_ANCHOR;
    }
  }

  fwrite(bwt_anchor, sizeof(bwt_anchor_t), num_items, fd);
  
  //free(buffer);  
  
}

void file_write_type_items(int type, FILE *fd) {  
  fwrite(&type, sizeof(int), 1, fd);
}

int file_read_type_items(FILE *fd) {
  int type;

  int bytes = fread(&type, sizeof(int), 1, fd);

  return bytes == 0 ? -1 : type;
}


void file_write_fastq_read(fastq_read_t *fq_read, size_t num_items, FILE *fd) {
  size_t head_size = strlen(fq_read->id);
  size_t seq_size  = fq_read->length;

  //Write binary file 
  //[type][size head][size seq][num items][HEAD][SEQUENCE][QUALITY][CAL 0][CAL n]
  size_t items_sizes[3] = {head_size, seq_size, num_items};
  //printf("NUM items %i\n", num_items);
  //printf("Insert-id  (%i): %s\n", head_size, fq_read->id);
  //printf("Insert-seq (%i): %s\n", seq_size, fq_read->sequence);
  //printf("Insert-qua (%i): %s\n", seq_size, fq_read->quality);

  //[size head][size seq][num items]
  fwrite(items_sizes, sizeof(size_t), 3, fd);
  //fwrite(&head_size, sizeof(size_t), 1, f_sa);
  //fwrite(&seq_size,  sizeof(size_t), 1, f_sa);
  //fwrite(&num_items, sizeof(size_t), 1, f_sa);
  
  //[HEAD][SEQUENCE][QUALITY]
  size_t total_size = head_size + 2*seq_size;
  char *buffer = (char *)malloc(sizeof(char)*total_size);
  
  memcpy(buffer, fq_read->id, head_size);
  memcpy(&buffer[head_size], fq_read->sequence, seq_size);
  memcpy(&buffer[head_size + seq_size], fq_read->quality, seq_size);

  fwrite(buffer, sizeof(char), total_size, fd);

  free(buffer);

}

void file_write_items(fastq_read_t *fq_read, array_list_t *items, 
		      unsigned char data_type, FILE *fd1, FILE *fd2,
		      int mode) {
  FILE *fd;

  if (mode == 0) {
    fd = fd1;
  } else {
    fd = fd2;
  }

  fwrite(&data_type, sizeof(unsigned char), 1, fd);
  file_write_fastq_read(fq_read, array_list_size(items), fd);

  if (data_type == CAL_TYPE) {
    //printf("======= INSERT CAL ITEMS (%i) ========\n", array_list_size(items));
    file_write_cals(fq_read, items, fd);
  } else if (data_type == META_ALIGNMENT_TYPE) {
    //printf("======= INSERT META ALIGNMENTS ITEMS (%i) ========\n", array_list_size(items));
    file_write_meta_alignments(fq_read, items, fd);    
  } else {
    //printf("======= INSERT ALIGNMENTS ITEMS (%i) ========\n", array_list_size(items));
    file_write_alignments(fq_read, items, fd);    
  }
  
}

//=================================================================
//File SA Functions

void sa_file_write_alignments(fastq_read_t *fq_read, array_list_t *items, FILE *fd) {
  //size_t head_size = strlen(fq_read->id);
  size_t seq_size  = fq_read->length;
  size_t num_items = array_list_size(items);
  if (!num_items) { 
    return;
  } 

  size_t max_len = num_items * 1024;
  char *cigar_buffer = (char *)calloc(max_len, sizeof(char));
  size_t tot_len = 0;

  simple_alignment_t simple_alignment[num_items];
  simple_alignment_t *simple_a;
  //unsigned char type = MENTA_TYPE;
  //fwrite(type, sizeof(unsigned char), 1, fd);

  memset(simple_alignment, 0, sizeof(simple_alignment_t)*num_items);
  //[type][size head][size seq][num items][HEAD][SEQUENCE][QUALITY][CAL 0][CAL n]
  for (int i = 0; i < num_items; i++) {
    sa_alignment_t *sa_alignment = array_list_get(i, items);
    cal_t *first_cal = array_list_get(0, sa_alignment->cals_list);
    cal_t *last_cal = array_list_get(sa_alignment->cals_list->size - 1, sa_alignment->cals_list);
    seed_region_t *first_seed = linked_list_get_first(first_cal->sr_list);
    seed_region_t *last_seed  = linked_list_get_last(last_cal->sr_list);
    cigar_code_t *cigar_code = sa_alignment->c_final;    
    char *cigar_str = new_cigar_code_string(cigar_code);
    if (cigar_str == NULL) { 
      cigar_str = (char *)calloc(1, sizeof(char));
    }
    int cigar_len = strlen(cigar_str);

    simple_a = &simple_alignment[i];

    simple_a->gap_start = first_seed->read_start;
    simple_a->gap_end = seq_size - last_seed->read_end - 1;


    simple_a->map_strand = first_cal->strand;
    simple_a->map_chromosome = first_cal->chromosome_id;
    simple_a->map_start = first_cal->start;
    simple_a->map_distance = cigar_code->distance;
    simple_a->cigar_len = cigar_len;
    
    //printf("==== Write to file ====\n");
    //cal_print(first_cal);
    //cal_print(last_cal);
    // printf("==== Write to file ====\n");
    //printf(" [%i:%lu] INSERT CIGAR(%i): %s\n", simple_a->map_chromosome, 
    //	   simple_a->map_start, cigar_len, cigar_str);

    memcpy(&cigar_buffer[tot_len], cigar_str, cigar_len);
    tot_len += cigar_len;

    if (tot_len >= max_len) { 
      max_len = max_len * 2;
      cigar_buffer = realloc(cigar_buffer, max_len); 
    }
    free(cigar_str);
  }

  fwrite(simple_alignment, sizeof(simple_alignment_t), num_items, fd);
  fwrite(cigar_buffer, sizeof(char), tot_len, fd);  

  free(cigar_buffer);

}

//-------------------------------------------------------------------------------

void sa_file_write_partial_alignments(fastq_read_t *fq_read, array_list_t *items, FILE *fd) {
  //size_t head_size = strlen(fq_read->id);
  size_t num_items = array_list_size(items);
  if (!num_items) { 
    return;
  } 

  size_t max_len = num_items * 1024;
  char *cigar_buffer = (char *)calloc(max_len, sizeof(char));
  size_t tot_len = 0;
  
  simple_alignment_t simple_alignment[num_items];
  
  simple_alignment_t *simple_a;
  
  //unsigned char type = MENTA_TYPE;
  //fwrite(type, sizeof(unsigned char), 1, fd);

  memset(simple_alignment, 0, sizeof(simple_alignment_t)*num_items);
  //[type][size head][size seq][num items][HEAD][SEQUENCE][QUALITY][CAL 0][CAL n]
  for (int i = 0; i < num_items; i++) {
    sa_alignment_partial_t *sa_partial_alignment = array_list_get(i, items);
    char *cigar_str = sa_partial_alignment->cigar;
    int cigar_len = sa_partial_alignment->cigar_len;

    simple_a = &simple_alignment[i];

    simple_a->gap_start = sa_partial_alignment->gap_start;
    simple_a->gap_end = sa_partial_alignment->gap_end;
    simple_a->map_strand = sa_partial_alignment->map_strand;
    simple_a->map_chromosome = sa_partial_alignment->map_chromosome;
    simple_a->map_start = sa_partial_alignment->map_start;
    simple_a->map_distance = sa_partial_alignment->map_distance;
    simple_a->cigar_len = cigar_len;
    

    memcpy(&cigar_buffer[tot_len], cigar_str, cigar_len);
    tot_len += cigar_len;

    if (tot_len >= max_len) { 
      max_len = max_len * 2;
      cigar_buffer = realloc(cigar_buffer, max_len); 
    }
  }

  fwrite(simple_alignment, sizeof(simple_alignment_t), num_items, fd);
  fwrite(cigar_buffer, sizeof(char), tot_len, fd);  

  free(cigar_buffer);

}

//-------------------------------------------------------------------------------

alignment_data_t *sa_file_read_alignments(size_t num_items, array_list_t *list, 
					  fastq_read_t *fq_read, FILE *fd) {
  
  if (!num_items) { return 0; }
    
  int bytes;

  alignment_data_t *p = alignment_data_new();
  p->num_items = num_items;
  p->simple_alignments_array = (simple_alignment_t *)malloc(sizeof(simple_alignment_t)*num_items);
  
  //for (int i = 0; i < num_items; i++) {
  //p->simple_alignments_array[i] = (simple_alignment_t *)malloc(sizeof(simple_alignment_t));
  //}

  //start_timer(time_start);
  
  //bytes = fread(simple_alignment, sizeof(simple_alignment_t), num_items, fd);
  bytes = fread(p->simple_alignments_array, sizeof(simple_alignment_t), num_items, fd);

  if (!bytes) { LOG_FATAL("Corrupt file\n"); }
  
  size_t cigar_tot_len = 0;
  for (int i = 0; i < num_items; i++) {
    //simple_a = &simple_alignment[i];
    //printf("ITEM %i: (%i)[%i:%lu] [%i-%i]\n", i, simple_a->map_strand, simple_a->map_chromosome,
    //	   simple_a->map_start, simple_a->gap_start, simple_a->gap_end);
    cigar_tot_len += p->simple_alignments_array[i].cigar_len;
  } 
 
  //char cigar_buffer[cigar_tot_len];
  p->cigars_str = (char *)malloc(sizeof(char)*cigar_tot_len);

  //bytes = fread(cigar_buffer, sizeof(char), cigar_tot_len, fd);
  bytes = fread(p->cigars_str, sizeof(char), cigar_tot_len, fd);
  if (!bytes) { LOG_FATAL("Corrupt file\n"); }
  //stop_timer(time_start, time_end, time_read_alig);

  array_list_insert(p, list);

  return p;
  //return 0;

}

//-------------------------------------------------------------------------------

void sa_file_write_items(int type, fastq_read_t *fq_read, array_list_t *items, FILE *fd) {
  file_write_type_items(type, fd);
  file_write_fastq_read(fq_read, array_list_size(items), fd);
  if (type == SA_PARTIAL_TYPE) {
    sa_file_write_partial_alignments(fq_read, items, fd);  
  } else {
    file_write_alignments(fq_read, items, fd);
  }
  //sa_file_write_alignments(fq_read, items, fd);  
}

//=================================================================



sa_alignment_partial_t *sa_alignment_partial_new(sa_alignment_t *sa_alignment, int seq_size) {
  sa_alignment_partial_t *p = (sa_alignment_partial_t *)malloc(sizeof(sa_alignment_partial_t));
  
  cal_t *first_cal = array_list_get(0, sa_alignment->cals_list);
  cal_t *last_cal = array_list_get(sa_alignment->cals_list->size - 1, sa_alignment->cals_list);
  seed_region_t *first_seed = linked_list_get_first(first_cal->sr_list);
  seed_region_t *last_seed  = linked_list_get_last(last_cal->sr_list);
  cigar_code_t *cigar_code = sa_alignment->c_final;    
  char *cigar_str = new_cigar_code_string(cigar_code);
  
  if (cigar_str == NULL) { 
    cigar_str = (char *)calloc(1, sizeof(char));
  }
  
  int cigar_len = strlen(cigar_str);
  
  p->gap_start = first_seed->read_start;
  p->gap_end = seq_size - last_seed->read_end - 1;
  
  p->map_strand = first_cal->strand;
  p->map_chromosome = first_cal->chromosome_id;
  p->map_start = first_cal->start;
  p->map_distance = cigar_code->distance;
  p->cigar_len = cigar_len;
  p->cigar = cigar_str;
  
  return p;
  
}

void sa_alignment_partial_free(sa_alignment_partial_t *sa_alignment_p) {
  free(sa_alignment_p->cigar);
  free(sa_alignment_p);
}