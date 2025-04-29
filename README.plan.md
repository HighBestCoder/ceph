
# 把osd.16加回去

ceph auth import -i /var/lib/ceph/osd/ceph-16/keyring

# 或者手动添加
# ceph auth add osd.16 osd 'allow *' mon 'allow profile osd' -i /var/lib/ceph/osd/ceph-16/keyring


ceph osd create 16

ceph osd crush add osd.16 0.0 host=control02

然后再启动osd.16


如果没有问题，那么设置nobackfill，然后问一下客户那三个image是否可以删除？如果可以，那么删除掉。

最后一招，设置size = 1 先把服务带起来

# OSD 23 修复


Osd.23 在backup fill pos

```
523014    -22> 2025-04-28T12:54:48.633+0800 fffd1122c880  0 log_channel(cluster) log [INF] : 7.418 continuing backfill to osd.7 from (31228'2146834,55960'2147762] 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head to 55960'2147762


backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head

   -12> 2025-04-28T12:54:48.743+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69458) [7]/[23] backfill=[7] r=0 lpr=69462 pi=[30860,69458)/4 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
    -4> 2025-04-28T12:58:23.863+0800 fffc6a2ac880  5 osd.23 pg_epoch: 69521 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69520) [7]/[23] backfill=[7] r=0 lpr=69521 pi=[30860,69520)/5 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
    -2> 2025-04-28T13:00:46.633+0800 fffcd664c880  5 osd.23 pg_epoch: 69548 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69547) [7]/[23] backfill=[7] r=0 lpr=69548 pi=[30860,69547)/5 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
    -2> 2025-04-28T13:03:12.023+0800 fffdd07cc880  5 osd.23 pg_epoch: 69580 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69579) [7]/[23] backfill=[7] r=0 lpr=69580 pi=[30860,69579)/5 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
    -2> 2025-04-28T13:06:02.243+0800 fffd16d7c880  5 osd.23 pg_epoch: 69619 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69616/69617 n=1665 ec=6913/86 lis/c=69616/30860 les/c/f=69617/30861/28829 sis=69618) [7]/[23] backfill=[7] r=0 lpr=69619 pi=[30860,69618)/5 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
    -2> 2025-04-28T13:08:25.823+0800 fffefbbbc880  5 osd.23 pg_epoch: 69637 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69616/69617 n=1665 ec=6913/86 lis/c=69616/30860 les/c/f=69617/30861/28829 sis=69636) [7]/[23] backfill=[7] r=0 lpr=69637 pi=[30860,69636)/5 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
    -6> 2025-04-28T13:11:36.513+0800 fffe0ee2c880  5 osd.23 pg_epoch: 69672 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69669/69670 n=1665 ec=6913/86 lis/c=69669/30860 les/c/f=69670/30861/28829 sis=69671) [7]/[23] backfill=[7] r=0 lpr=69672 pi=[30860,69671)/5 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
    -3> 2025-04-28T13:13:58.663+0800 fffe4c0bc880  5 osd.23 pg_epoch: 69696 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69669/69670 n=1665 ec=6913/86 lis/c=69669/30860 les/c/f=69670/30861/28829 sis=69695) [7]/[23] backfill=[7] r=0 lpr=69696 pi=[30860,69695)/5 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
    -6> 2025-04-28T13:16:19.733+0800 ffff3707c880  5 osd.23 pg_epoch: 69720 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69669/69670 n=1665 ec=6913/86 lis/c=69669/30860 les/c/f=69670/30861/28829 sis=69719) [7]/[23] backfill=[7] r=0 lpr=69720 pi=[30860,69719)/5 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
    -3> 2025-04-28T13:18:50.023+0800 fffe4ad9c880  5 osd.23 pg_epoch: 69744 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69669/69670 n=1665 ec=6913/86 lis/c=69669/30860 les/c/f=69670/30861/28829 sis=69743) [7]/[23] backfill=[7] r=0 lpr=69744 pi=[30860,69743)/5 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
   -11> 2025-04-28T13:23:16.553+0800 fffc7876c880  5 osd.23 pg_epoch: 69783 pg[7.418( v 69781'2147763 (31228'2146834,69781'2147763] local-lis/les=69782/69783 n=1665 ec=6913/86 lis/c=69782/30860 les/c/f=69783/30861/28829 sis=69782) [7]/[23] backfill=[7] r=0 lpr=69783 pi=[30860,69782)/6 crt=69781'2147763 lcod 55960'2147762 mlcod 0'0 active+undersized+degraded+remapped+backfilling pruub 126.966163635s@5 rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head

   从日志中找到是 7.418
```

# osd 16

# OSD 23
-21> 2025-04-28T12:54:48.633+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69458) [7]/[23] backfill=[7] r=0 lpr=69462 pi=[30860,69458)/4 crt=55960'2147762 lcod 0'0 mlcod 0'0 activating+undersized+degraded+remapped mbc={} trimq=[c17~1,c1b~1,c1f~1]] enter Started/Primary/Active/Activating
   -20> 2025-04-28T12:54:48.633+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69458) [7]/[23] backfill=[7] r=0 lpr=69462 pi=[30860,69458)/4 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+peered mbc={} trimq=[c17~1,c1b~1,c1f~1]] on_activate_complete: bft=7 from 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
   -19> 2025-04-28T12:54:48.633+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69458) [7]/[23] backfill=[7] r=0 lpr=69462 pi=[30860,69458)/4 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+peered mbc={} trimq=[c17~1,c1b~1,c1f~1]] target shard 7 from 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
   -18> 2025-04-28T12:54:48.633+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69458) [7]/[23] backfill=[7] r=0 lpr=69462 pi=[30860,69458)/4 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+peered mbc={} trimq=[c17~1,c1b~1,c1f~1]] exit Started/Primary/Active/Activating 0.001802 3 0.000461
   -17> 2025-04-28T12:54:48.633+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69458) [7]/[23] backfill=[7] r=0 lpr=69462 pi=[30860,69458)/4 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+peered mbc={} trimq=[c17~1,c1b~1,c1f~1]] enter Started/Primary/Active/WaitLocalBackfillReserved
   -16> 2025-04-28T12:54:48.633+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69458) [7]/[23] backfill=[7] r=0 lpr=69462 pi=[30860,69458)/4 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfill_wait+peered mbc={} trimq=[c17~1,c1b~1,c1f~1]] exit Started/Primary/Active/WaitLocalBackfillReserved 0.000109 1 0.000082
   -15> 2025-04-28T12:54:48.633+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69458) [7]/[23] backfill=[7] r=0 lpr=69462 pi=[30860,69458)/4 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfill_wait+peered mbc={} trimq=[c17~1,c1b~1,c1f~1]] enter Started/Primary/Active/WaitRemoteBackfillReserved
   -14> 2025-04-28T12:54:48.633+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69458) [7]/[23] backfill=[7] r=0 lpr=69462 pi=[30860,69458)/4 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfill_wait+peered mbc={} trimq=[c17~1,c1b~1,c1f~1]] exit Started/Primary/Active/WaitRemoteBackfillReserved 0.000937 1 0.000059
   -13> 2025-04-28T12:54:48.633+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69458) [7]/[23] backfill=[7] r=0 lpr=69462 pi=[30860,69458)/4 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfill_wait+peered mbc={} trimq=[c17~1,c1b~1,c1f~1]] enter Started/Primary/Active/Backfilling
   -12> 2025-04-28T12:54:48.743+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[7.418( v 55960'2147762 (31228'2146834,55960'2147762] local-lis/les=69451/69455 n=1665 ec=6913/86 lis/c=69451/30860 les/c/f=69455/30861/28829 sis=69458) [7]/[23] backfill=[7] r=0 lpr=69462 pi=[30860,69458)/4 crt=55960'2147762 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:1832eb68:::rbd_data.5426f358a0edb.000000000001b6c5:head
   -11> 2025-04-28T12:54:48.743+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[6.62( v 31074'1882 (2817'1135,31074'1882] local-lis/les=69451/69455 n=29 ec=3577/76 lis/c=69434/69434 les/c/f=69438/69438/28829 sis=69458) [11,23] r=1 lpr=69458 pi=[69434,69458)/1 crt=31074'1882 mlcod 0'0 unknown NOTIFY mbc={}] exit Started/Stray 8.694047 15 0.000193
   -10> 2025-04-28T12:54:48.743+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[6.62( v 31074'1882 (2817'1135,31074'1882] local-lis/les=69451/69455 n=29 ec=3577/76 lis/c=69434/69434 les/c/f=69438/69438/28829 sis=69458) [11,23] r=1 lpr=69458 pi=[69434,69458)/1 crt=31074'1882 mlcod 0'0 unknown NOTIFY mbc={}] enter Started/ReplicaActive
    -9> 2025-04-28T12:54:48.743+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[6.62( v 31074'1882 (2817'1135,31074'1882] local-lis/les=69451/69455 n=29 ec=3577/76 lis/c=69434/69434 les/c/f=69438/69438/28829 sis=69458) [11,23] r=1 lpr=69458 pi=[69434,69458)/1 crt=31074'1882 mlcod 0'0 unknown NOTIFY mbc={}] enter Started/ReplicaActive/RepNotRecovering
    -8> 2025-04-28T12:54:48.743+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[6.f8( v 55075'1831 (2817'1062,55075'1831] local-lis/les=69458/69462 n=40 ec=4574/76 lis/c=69458/69451 les/c/f=69462/69455/28829 sis=69458) [23,11] r=0 lpr=69458 pi=[69451,69458)/1 crt=55075'1831 lcod 0'0 mlcod 0'0 active+undersized+degraded mbc={}] exit Started/Primary/Active/Activating 0.115794 4 0.000533
    -7> 2025-04-28T12:54:48.743+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[6.f8( v 55075'1831 (2817'1062,55075'1831] local-lis/les=69458/69462 n=40 ec=4574/76 lis/c=69458/69451 les/c/f=69462/69455/28829 sis=69458) [23,11] r=0 lpr=69458 pi=[69451,69458)/1 crt=55075'1831 lcod 0'0 mlcod 0'0 active+undersized+degraded mbc={}] enter Started/Primary/Active/Recovered
    -6> 2025-04-28T12:54:48.743+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[6.f8( v 55075'1831 (2817'1062,55075'1831] local-lis/les=69458/69462 n=40 ec=4574/76 lis/c=69458/69451 les/c/f=69462/69455/28829 sis=69458) [23,11] r=0 lpr=69458 pi=[69451,69458)/1 crt=55075'1831 lcod 0'0 mlcod 0'0 active+undersized+degraded mbc={}] exit Started/Primary/Active/Recovered 0.000016 0 0.000000
    -5> 2025-04-28T12:54:48.743+0800 fffd1122c880  5 osd.23 pg_epoch: 69462 pg[6.f8( v 55075'1831 (2817'1062,55075'1831] local-lis/les=69458/69462 n=40 ec=4574/76 lis/c=69458/69451 les/c/f=69462/69455/28829 sis=69458) [23,11] r=0 lpr=69458 pi=[69451,69458)/1 crt=55075'1831 lcod 0'0 mlcod 0'0 active+undersized+degraded mbc={}] enter Started/Primary/Active/Clean
    -4> 2025-04-28T12:54:48.773+0800 fffd27d2c880 -1 osd.23 69462 heartbeat_check: no reply from 192.168.26.62:6860 osd.16 since back 2025-04-28T12:53:18.142303+0800 front 2025-04-28T12:53:18.142376+0800 (oldest deadline 2025-04-28T12:53:43.441974+0800)
    -3> 2025-04-28T12:54:48.773+0800 fffd27d2c880 -1 osd.23 69462 get_health_metrics reporting 3 slow ops, oldest is osd_op(client.8619502.0:1710972 4.9 4:93e5b521:::notify.7:head [watch watch cookie 281425450384736] snapc 0=[] ondisk+write+known_if_redirected e69369)
    -2> 2025-04-28T12:54:48.773+0800 fffd27d2c880  0 log_channel(cluster) log [WRN] : 3 slow requests (by type [ 'delayed' : 3 ] most affected pool [ 'default.rgw.control' : 3 ])
    -1> 2025-04-28T12:54:48.813+0800 fffd1122c880 -1 /builddir/build/BUILD/ceph-16.2.12/src/osd/osd_types.cc: In function 'uint64_t SnapSet::get_clone_bytes(snapid_t) const' thread fffd1122c880 time 2025-04-28T12:54:48.821797+0800
/builddir/build/BUILD/ceph-16.2.12/src/osd/osd_types.cc: 5794: FAILED ceph_assert(clone_overlap.count(clone))

 ceph version 16.2.12 (e37f34a69800de20630e6ec783964c27cb3ea547) pacific (stable)
 1: (ceph::__ceph_assert_fail(char const*, char const*, int, char const*)+0x128) [0xaaae0edd7188]
 2: (ceph::__ceph_assertf_fail(char const*, char const*, int, char const*, char const*, ...)+0) [0xaaae0edd72f8]
 3: (SnapSet::get_clone_bytes(snapid_t) const+0xdc) [0xaaae0f01af3c]
 4: (PrimaryLogPG::add_object_context_to_pg_stat(std::shared_ptr<ObjectContext>, pg_stat_t*)+0x10c) [0xaaae0ef9b6ec]
 5: (PrimaryLogPG::recover_backfill(unsigned long, ThreadPool::TPHandle&, bool*)+0xc98) [0xaaae0efaf218]
 6: (PrimaryLogPG::start_recovery_ops(unsigned long, ThreadPool::TPHandle&, unsigned long*)+0xcc4) [0xaaae0efa5d34]
 7: (OSD::do_recovery(PG*, unsigned int, unsigned long, ThreadPool::TPHandle&)+0x228) [0xaaae0eeb2f48]
 8: (ceph::osd::scheduler::PGRecovery::run(OSD*, OSDShard*, boost::intrusive_ptr<PG>&, ThreadPool::TPHandle&)+0x28) [0xaaae0f031088]
 9: (OSD::ShardedOpWQ::_process(unsigned int, ceph::heartbeat_handle_d*)+0x63c) [0xaaae0eec43c0]
 10: (ShardedThreadPool::shardedthreadpool_worker(unsigned int)+0x23c) [0xaaae0f3284ec]
 11: /usr/bin/ceph-osd(+0x8689e8) [0xaaae0f3289e8]
 12: /lib64/libc.so.6(+0x82a68) [0xfffd2dca2a68]
 13: /lib64/libc.so.6(+0x2bb9c) [0xfffd2dc4bb9c]


# OSD 16
#
8T11:02:04.689+0800 fffbecf4c880  0 log_channel(cluster) log [WRN] : 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
  -106> 2025-04-28T11:02:04.919+0800 fffbe4fdc880 10 log_client handle_log_ack log(last 5) v1
  -105> 2025-04-28T11:02:04.919+0800 fffbe4fdc880 10 log_client  logged 2025-04-28T11:02:03.720440+0800 osd.16 (osd.16) 5 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
  -104> 2025-04-28T11:02:05.609+0800 fffbe3fbc880 10 monclient: tick
  -103> 2025-04-28T11:02:05.609+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:35.624910+0800)
  -102> 2025-04-28T11:02:05.609+0800 fffbe3fbc880 10 log_client  log_queue is 1 last_log 6 sent 5 num 1 unsent 1 sending 1
  -101> 2025-04-28T11:02:05.609+0800 fffbe3fbc880 10 log_client  will send 2025-04-28T11:02:04.704332+0800 osd.16 (osd.16) 6 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
  -100> 2025-04-28T11:02:05.609+0800 fffbe3fbc880 10 monclient: _send_mon_message to mon.192.168.26.62 at v2:192.168.26.62:3300/0
   -99> 2025-04-28T11:02:05.669+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1234247680 unmapped: 237568 heap: 1234485248 old mem: 2845415832 new mem: 2845415832
   -98> 2025-04-28T11:02:05.709+0800 fffbecf4c880 -1 osd.16 68922 get_health_metrics reporting 6 slow ops, oldest is osd_op(client.8619502.0:1701623 4.15 4:a93a5511:::notify.2:head [watch reconnect cookie 281425450371936 gen 1827] snapc 0=[] ondisk+write+known_if_redirected e68913)
   -97> 2025-04-28T11:02:05.709+0800 fffbecf4c880  0 log_channel(cluster) log [WRN] : 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -96> 2025-04-28T11:02:05.929+0800 fffbe4fdc880 10 log_client handle_log_ack log(last 6) v1
   -95> 2025-04-28T11:02:05.929+0800 fffbe4fdc880 10 log_client  logged 2025-04-28T11:02:04.704332+0800 osd.16 (osd.16) 6 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -94> 2025-04-28T11:02:06.149+0800 fffbd644c880  5 osd.16 pg_epoch: 68922 pg[7.247( v 64309'906488 (31225'905686,64309'906488] local-lis/les=68913/68920 n=1674 ec=5908/86 lis/c=68913/68903 les/c/f=68920/68910/28829 sis=68913) [16,14] r=0 lpr=68913 pi=[68903,68913)/1 crt=64309'906488 lcod 0'0 mlcod 0'0 active mbc={} ps=[c17~1,c1b~1,c1f~1]] exit Started/Primary/Active/Activating 24.918071 12 0.000899
   -93> 2025-04-28T11:02:06.149+0800 fffbd644c880  5 osd.16 pg_epoch: 68922 pg[7.247( v 64309'906488 (31225'905686,64309'906488] local-lis/les=68913/68920 n=1674 ec=5908/86 lis/c=68913/68903 les/c/f=68920/68910/28829 sis=68913) [16,14] r=0 lpr=68913 pi=[68903,68913)/1 crt=64309'906488 lcod 0'0 mlcod 0'0 active mbc={} ps=[c17~1,c1b~1,c1f~1]] enter Started/Primary/Active/Recovered
   -92> 2025-04-28T11:02:06.149+0800 fffbd644c880  5 osd.16 pg_epoch: 68922 pg[7.247( v 64309'906488 (31225'905686,64309'906488] local-lis/les=68913/68920 n=1674 ec=5908/86 lis/c=68913/68903 les/c/f=68920/68910/28829 sis=68913) [16,14] r=0 lpr=68913 pi=[68903,68913)/1 crt=64309'906488 lcod 0'0 mlcod 0'0 active mbc={} ps=[c17~1,c1b~1,c1f~1]] exit Started/Primary/Active/Recovered 0.000025 0 0.000000
   -91> 2025-04-28T11:02:06.149+0800 fffbd644c880  5 osd.16 pg_epoch: 68922 pg[7.247( v 64309'906488 (31225'905686,64309'906488] local-lis/les=68913/68920 n=1674 ec=5908/86 lis/c=68913/68903 les/c/f=68920/68910/28829 sis=68913) [16,14] r=0 lpr=68913 pi=[68903,68913)/1 crt=64309'906488 lcod 0'0 mlcod 0'0 active mbc={} ps=[c17~1,c1b~1,c1f~1]] enter Started/Primary/Active/Clean
   -90> 2025-04-28T11:02:06.609+0800 fffbe3fbc880 10 monclient: tick
   -89> 2025-04-28T11:02:06.609+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:36.625153+0800)
   -88> 2025-04-28T11:02:06.609+0800 fffbe3fbc880 10 log_client  log_queue is 1 last_log 7 sent 6 num 1 unsent 1 sending 1
   -87> 2025-04-28T11:02:06.609+0800 fffbe3fbc880 10 log_client  will send 2025-04-28T11:02:05.723893+0800 osd.16 (osd.16) 7 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -86> 2025-04-28T11:02:06.609+0800 fffbe3fbc880 10 monclient: _send_mon_message to mon.192.168.26.62 at v2:192.168.26.62:3300/0
   -85> 2025-04-28T11:02:06.669+0800 fffbecf4c880 -1 osd.16 68922 get_health_metrics reporting 6 slow ops, oldest is osd_op(client.8619502.0:1701623 4.15 4:a93a5511:::notify.2:head [watch reconnect cookie 281425450371936 gen 1827] snapc 0=[] ondisk+write+known_if_redirected e68913)
   -84> 2025-04-28T11:02:06.669+0800 fffbecf4c880  0 log_channel(cluster) log [WRN] : 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -83> 2025-04-28T11:02:06.679+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1234264064 unmapped: 221184 heap: 1234485248 old mem: 2845415832 new mem: 2845415832
   -82> 2025-04-28T11:02:06.929+0800 fffbe4fdc880 10 log_client handle_log_ack log(last 7) v1
   -81> 2025-04-28T11:02:06.929+0800 fffbe4fdc880 10 log_client  logged 2025-04-28T11:02:05.723893+0800 osd.16 (osd.16) 7 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -80> 2025-04-28T11:02:07.039+0800 fffbd542c880  5 osd.16 68922 heartbeat osd_stat(store_statfs(0x5ffb5afc000/0x0/0x746dbafb000, data 0x14c1ff1d732/0x14596ceb000, compress 0x0/0x0/0x0, omap 0x281e4ea, meta 0x18caf1b16), peers [7,9,11,13,14,15,17,23,24,25,26,27,28,29] op hist [0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,6])
   -79> 2025-04-28T11:02:07.609+0800 fffbe3fbc880 10 monclient: tick
   -78> 2025-04-28T11:02:07.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:37.625459+0800)
   -77> 2025-04-28T11:02:07.619+0800 fffbe3fbc880 10 log_client  log_queue is 1 last_log 8 sent 7 num 1 unsent 1 sending 1
   -76> 2025-04-28T11:02:07.619+0800 fffbe3fbc880 10 log_client  will send 2025-04-28T11:02:06.681310+0800 osd.16 (osd.16) 8 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -75> 2025-04-28T11:02:07.619+0800 fffbe3fbc880 10 monclient: _send_mon_message to mon.192.168.26.62 at v2:192.168.26.62:3300/0
   -74> 2025-04-28T11:02:07.659+0800 fffbecf4c880 -1 osd.16 68922 get_health_metrics reporting 6 slow ops, oldest is osd_op(client.8619502.0:1701623 4.15 4:a93a5511:::notify.2:head [watch reconnect cookie 281425450371936 gen 1827] snapc 0=[] ondisk+write+known_if_redirected e68913)
   -73> 2025-04-28T11:02:07.659+0800 fffbecf4c880  0 log_channel(cluster) log [WRN] : 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -72> 2025-04-28T11:02:07.679+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1234305024 unmapped: 180224 heap: 1234485248 old mem: 2845415832 new mem: 2845415832
   -71> 2025-04-28T11:02:07.679+0800 fffbe5e3c880  5 bluestore.MempoolThread(0xaaabf8d8abe0) _resize_shards cache_size: 2845415832 kv_alloc: 1224736768 kv_used: 175117696 kv_onode_alloc: 335544320 kv_onode_used: 177637920 meta_alloc: 1040187392 meta_used: 379291 data_alloc: 201326592 data_used: 278528
   -70> 2025-04-28T11:02:07.929+0800 fffbe4fdc880 10 log_client handle_log_ack log(last 8) v1
   -69> 2025-04-28T11:02:07.929+0800 fffbe4fdc880 10 log_client  logged 2025-04-28T11:02:06.681310+0800 osd.16 (osd.16) 8 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -68> 2025-04-28T11:02:08.619+0800 fffbe3fbc880 10 monclient: tick
   -67> 2025-04-28T11:02:08.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:38.625720+0800)
   -66> 2025-04-28T11:02:08.619+0800 fffbe3fbc880 10 log_client  log_queue is 1 last_log 9 sent 8 num 1 unsent 1 sending 1
   -65> 2025-04-28T11:02:08.619+0800 fffbe3fbc880 10 log_client  will send 2025-04-28T11:02:07.672891+0800 osd.16 (osd.16) 9 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -64> 2025-04-28T11:02:08.619+0800 fffbe3fbc880 10 monclient: _send_mon_message to mon.192.168.26.62 at v2:192.168.26.62:3300/0
   -63> 2025-04-28T11:02:08.649+0800 fffbecf4c880 -1 osd.16 68922 get_health_metrics reporting 6 slow ops, oldest is osd_op(client.8619502.0:1701623 4.15 4:a93a5511:::notify.2:head [watch reconnect cookie 281425450371936 gen 1827] snapc 0=[] ondisk+write+known_if_redirected e68913)
   -62> 2025-04-28T11:02:08.649+0800 fffbecf4c880  0 log_channel(cluster) log [WRN] : 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -61> 2025-04-28T11:02:08.679+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1234313216 unmapped: 172032 heap: 1234485248 old mem: 2845415832 new mem: 2845415832
   -60> 2025-04-28T11:02:08.939+0800 fffbe4fdc880 10 log_client handle_log_ack log(last 9) v1
   -59> 2025-04-28T11:02:08.939+0800 fffbe4fdc880 10 log_client  logged 2025-04-28T11:02:07.672891+0800 osd.16 (osd.16) 9 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -58> 2025-04-28T11:02:09.619+0800 fffbe3fbc880 10 monclient: tick
   -57> 2025-04-28T11:02:09.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:39.625956+0800)
   -56> 2025-04-28T11:02:09.619+0800 fffbe3fbc880 10 log_client  log_queue is 1 last_log 10 sent 9 num 1 unsent 1 sending 1
   -55> 2025-04-28T11:02:09.619+0800 fffbe3fbc880 10 log_client  will send 2025-04-28T11:02:08.661854+0800 osd.16 (osd.16) 10 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -54> 2025-04-28T11:02:09.619+0800 fffbe3fbc880 10 monclient: _send_mon_message to mon.192.168.26.62 at v2:192.168.26.62:3300/0
   -53> 2025-04-28T11:02:09.669+0800 fffbd644c880  5 osd.16 pg_epoch: 68922 pg[7.17f( v 55866'4292090 (31228'4291127,55866'4292090] local-lis/les=30710/30711 n=1610 ec=5296/86 lis/c=30710/30710 les/c/f=30711/30711/28829 sis=68921) [16,9]/[16] backfill=[9] r=0 lpr=68922 pi=[30710,68921)/6 crt=55866'4292090 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfill_wait+peered mbc={} trimq=[c17~1,c1b~1,c1f~1]] exit Started/Primary/Active/WaitRemoteBackfillReserved 10.703762 2 0.000110
   -52> 2025-04-28T11:02:09.669+0800 fffbd644c880  5 osd.16 pg_epoch: 68922 pg[7.17f( v 55866'4292090 (31228'4291127,55866'4292090] local-lis/les=30710/30711 n=1610 ec=5296/86 lis/c=30710/30710 les/c/f=30711/30711/28829 sis=68921) [16,9]/[16] backfill=[9] r=0 lpr=68922 pi=[30710,68921)/6 crt=55866'4292090 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfill_wait+peered mbc={} trimq=[c17~1,c1b~1,c1f~1]] enter Started/Primary/Active/Backfilling
   -51> 2025-04-28T11:02:09.679+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1234419712 unmapped: 1114112 heap: 1235533824 old mem: 2845415832 new mem: 2845415832
   -50> 2025-04-28T11:02:09.689+0800 fffbecf4c880 -1 osd.16 68922 get_health_metrics reporting 6 slow ops, oldest is osd_op(client.8619502.0:1701623 4.15 4:a93a5511:::notify.2:head [watch reconnect cookie 281425450371936 gen 1827] snapc 0=[] ondisk+write+known_if_redirected e68913)
   -49> 2025-04-28T11:02:09.689+0800 fffbecf4c880  0 log_channel(cluster) log [WRN] : 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -48> 2025-04-28T11:02:09.819+0800 fffbd644c880  5 osd.16 pg_epoch: 68922 pg[7.17f( v 55866'4292090 (31228'4291127,55866'4292090] local-lis/les=30710/30711 n=1610 ec=5296/86 lis/c=30710/30710 les/c/f=30711/30711/28829 sis=68921) [16,9]/[16] backfill=[9] r=0 lpr=68922 pi=[30710,68921)/6 crt=55866'4292090 lcod 0'0 mlcod 0'0 undersized+degraded+remapped+backfilling+peered rops=1 mbc={} trimq=[c17~1,c1b~1,c1f~1]] backfill_pos is 7:fe931189:::rbd_data.17e53ebb72011.000000000000238f:head
   -47> 2025-04-28T11:02:09.939+0800 fffbe4fdc880 10 log_client handle_log_ack log(last 10) v1
   -46> 2025-04-28T11:02:09.939+0800 fffbe4fdc880 10 log_client  logged 2025-04-28T11:02:08.661854+0800 osd.16 (osd.16) 10 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -45> 2025-04-28T11:02:10.419+0800 fffbe2ddc880  5 bluestore(/var/lib/ceph/osd/ceph-16) _kv_sync_thread utilization: idle 11.457093239s of 11.464491844s, submitted: 15
   -44> 2025-04-28T11:02:10.619+0800 fffbe3fbc880 10 monclient: tick
   -43> 2025-04-28T11:02:10.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:40.626203+0800)
   -42> 2025-04-28T11:02:10.619+0800 fffbe3fbc880 10 log_client  log_queue is 1 last_log 11 sent 10 num 1 unsent 1 sending 1
   -41> 2025-04-28T11:02:10.619+0800 fffbe3fbc880 10 log_client  will send 2025-04-28T11:02:09.696993+0800 osd.16 (osd.16) 11 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -40> 2025-04-28T11:02:10.619+0800 fffbe3fbc880 10 monclient: _send_mon_message to mon.192.168.26.62 at v2:192.168.26.62:3300/0
   -39> 2025-04-28T11:02:10.689+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1242652672 unmapped: 221184 heap: 1242873856 old mem: 2845415832 new mem: 2845415832
   -38> 2025-04-28T11:02:10.949+0800 fffbe4fdc880 10 log_client handle_log_ack log(last 11) v1
   -37> 2025-04-28T11:02:10.949+0800 fffbe4fdc880 10 log_client  logged 2025-04-28T11:02:09.696993+0800 osd.16 (osd.16) 11 : cluster [WRN] 6 slow requests (by type [ 'started' : 6 ] most affected pool [ 'default.rgw.control' : 6 ])
   -36> 2025-04-28T11:02:11.619+0800 fffbe3fbc880 10 monclient: tick
   -35> 2025-04-28T11:02:11.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:41.626590+0800)
   -34> 2025-04-28T11:02:11.689+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1242685440 unmapped: 188416 heap: 1242873856 old mem: 2845415832 new mem: 2845415832
   -33> 2025-04-28T11:02:12.339+0800 fffbd542c880  5 osd.16 68922 heartbeat osd_stat(store_statfs(0x5ffb5afc000/0x0/0x746dbafb000, data 0x14c1ff1d732/0x14596ceb000, compress 0x0/0x0/0x0, omap 0x281e4ea, meta 0x18caf1b16), peers [7,9,11,13,14,15,17,23,24,25,26,27,28,29] op hist [])
   -32> 2025-04-28T11:02:12.619+0800 fffbe3fbc880 10 monclient: tick
   -31> 2025-04-28T11:02:12.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:42.626829+0800)
   -30> 2025-04-28T11:02:12.689+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1242824704 unmapped: 49152 heap: 1242873856 old mem: 2845415832 new mem: 2845415832
   -29> 2025-04-28T11:02:12.689+0800 fffbe5e3c880  5 bluestore.MempoolThread(0xaaabf8d8abe0) _resize_shards cache_size: 2845415832 kv_alloc: 1224736768 kv_used: 175117696 kv_onode_alloc: 335544320 kv_onode_used: 182276048 meta_alloc: 1040187392 meta_used: 1077421 data_alloc: 201326592 data_used: 278528
   -28> 2025-04-28T11:02:13.619+0800 fffbe3fbc880 10 monclient: tick
   -27> 2025-04-28T11:02:13.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:43.626989+0800)
   -26> 2025-04-28T11:02:13.689+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1242849280 unmapped: 24576 heap: 1242873856 old mem: 2845415832 new mem: 2845415832
   -25> 2025-04-28T11:02:14.619+0800 fffbe3fbc880 10 monclient: tick
   -24> 2025-04-28T11:02:14.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:44.627234+0800)
   -23> 2025-04-28T11:02:14.689+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1242865664 unmapped: 1056768 heap: 1243922432 old mem: 2845415832 new mem: 2845415832
   -22> 2025-04-28T11:02:15.619+0800 fffbe3fbc880 10 monclient: tick
   -21> 2025-04-28T11:02:15.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:45.627470+0800)
   -20> 2025-04-28T11:02:15.699+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1242898432 unmapped: 1024000 heap: 1243922432 old mem: 2845415832 new mem: 2845415832
   -19> 2025-04-28T11:02:16.619+0800 fffbe3fbc880 10 monclient: tick
   -18> 2025-04-28T11:02:16.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:46.627664+0800)
   -17> 2025-04-28T11:02:16.699+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1242906624 unmapped: 1015808 heap: 1243922432 old mem: 2845415832 new mem: 2845415832
   -16> 2025-04-28T11:02:17.039+0800 fffbd542c880  5 osd.16 68922 heartbeat osd_stat(store_statfs(0x5ffb5afc000/0x0/0x746dbafb000, data 0x14c1ff1d732/0x14596ceb000, compress 0x0/0x0/0x0, omap 0x281e4ea, meta 0x18caf1b16), peers [7,9,11,13,14,15,17,23,24,25,26,27,28,29] op hist [])
   -15> 2025-04-28T11:02:17.619+0800 fffbe3fbc880 10 monclient: tick
   -14> 2025-04-28T11:02:17.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:47.627913+0800)
   -13> 2025-04-28T11:02:17.699+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1242963968 unmapped: 958464 heap: 1243922432 old mem: 2845415832 new mem: 2845415832
   -12> 2025-04-28T11:02:17.699+0800 fffbe5e3c880  5 bluestore.MempoolThread(0xaaabf8d8abe0) _resize_shards cache_size: 2845415832 kv_alloc: 1224736768 kv_used: 175117696 kv_onode_alloc: 335544320 kv_onode_used: 182276048 meta_alloc: 1040187392 meta_used: 1077421 data_alloc: 201326592 data_used: 278528
   -11> 2025-04-28T11:02:18.619+0800 fffbe3fbc880 10 monclient: tick
   -10> 2025-04-28T11:02:18.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:48.628159+0800)
    -9> 2025-04-28T11:02:18.699+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1242963968 unmapped: 958464 heap: 1243922432 old mem: 2845415832 new mem: 2845415832
    -8> 2025-04-28T11:02:19.619+0800 fffbe3fbc880 10 monclient: tick
    -7> 2025-04-28T11:02:19.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:49.628329+0800)
    -6> 2025-04-28T11:02:19.699+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1242980352 unmapped: 942080 heap: 1243922432 old mem: 2845415832 new mem: 2845415832
    -5> 2025-04-28T11:02:20.539+0800 fffbd542c880  5 osd.16 68922 heartbeat osd_stat(store_statfs(0x5ffb5afc000/0x0/0x746dbafb000, data 0x14c1ff1d732/0x14596ceb000, compress 0x0/0x0/0x0, omap 0x281e4ea, meta 0x18caf1b16), peers [7,9,11,13,14,15,17,23,24,25,26,27,28,29] op hist [])
    -4> 2025-04-28T11:02:20.619+0800 fffbe3fbc880 10 monclient: tick
    -3> 2025-04-28T11:02:20.619+0800 fffbe3fbc880 10 monclient: _check_auth_rotating have uptodate secrets (they expire after 2025-04-28T11:01:50.628568+0800)
    -2> 2025-04-28T11:02:20.709+0800 fffbe5e3c880  5 prioritycache tune_memory target: 4294967296 mapped: 1243037696 unmapped: 884736 heap: 1243922432 old mem: 2845415832 new mem: 2845415832
    -1> 2025-04-28T11:02:21.549+0800 fffbd644c880 -1 /builddir/build/BUILD/ceph-16.2.12/src/osd/osd_types.cc: In function 'uint64_t SnapSet::get_clone_bytes(snapid_t) const' thread fffbd644c880 time 2025-04-28T11:02:21.558392+0800
/builddir/build/BUILD/ceph-16.2.12/src/osd/osd_types.cc: 5794: FAILED ceph_assert(clone_overlap.count(clone))

 ceph version 16.2.12 (e37f34a69800de20630e6ec783964c27cb3ea547) pacific (stable)
 1: (ceph::__ceph_assert_fail(char const*, char const*, int, char const*)+0x128) [0xaaabb8247188]
 2: (ceph::__ceph_assertf_fail(char const*, char const*, int, char const*, char const*, ...)+0) [0xaaabb82472f8]
 3: (SnapSet::get_clone_bytes(snapid_t) const+0xdc) [0xaaabb848af3c]
 4: (PrimaryLogPG::add_object_context_to_pg_stat(std::shared_ptr<ObjectContext>, pg_stat_t*)+0x10c) [0xaaabb840b6ec]
 5: (PrimaryLogPG::recover_backfill(unsigned long, ThreadPool::TPHandle&, bool*)+0xc98) [0xaaabb841f218]
 6: (PrimaryLogPG::start_recovery_ops(unsigned long, ThreadPool::TPHandle&, unsigned long*)+0xcc4) [0xaaabb8415d34]
 7: (OSD::do_recovery(PG*, unsigned int, unsigned long, ThreadPool::TPHandle&)+0x228) [0xaaabb8322f48]
 8: (ceph::osd::scheduler::PGRecovery::run(OSD*, OSDShard*, boost::intrusive_ptr<PG>&, ThreadPool::TPHandle&)+0x28) [0xaaabb84a1088]
 9: (OSD::ShardedOpWQ::_process(unsigned int, ceph::heartbeat_handle_d*)+0x63c) [0xaaabb83343c0]
 10: (ShardedThreadPool::shardedthreadpool_worker(unsigned int)+0x23c) [0xaaabb87984ec]
 11: /usr/bin/ceph-osd(+0x8689e8) [0xaaabb87989e8]
 12: /lib64/libc.so.6(+0x82a68) [0xfffbf2ec2a68]
 13: /lib64/libc.so.6(+0x2bb9c) [0xfffbf2e6bb9c]

     0> 2025-04-28T11:02:21.549+0800 fffbd644c880 -1 *** Caught signal (Aborted) **
 in thread fffbd644c880 thread_name:tp_osd_tp

 ceph version 16.2.12 (e37f34a69800de20630e6ec783964c27cb3ea547) pacific (stable)
 1: /usr/bin/ceph-osd(+0x83b34c) [0xaaabb876b34c]
 2: __kernel_rt_sigreturn()
 3: /lib64/libc.so.6(+0x84680) [0xfffbf2ec4680]
 4: raise()
 5: abort()
 6: (ceph::__ceph_assert_fail(char const*, char const*, int, char const*)+0x16c) [0xaaabb82471cc]
 7: (ceph::__ceph_assertf_fail(char const*, char const*, int, char const*, char const*, ...)+0) [0xaaabb82472f8]
 8: (SnapSet::get_clone_bytes(snapid_t) const+0xdc) [0xaaabb848af3c]
 9: (PrimaryLogPG::add_object_context_to_pg_stat(std::shared_ptr<ObjectContext>, pg_stat_t*)+0x10c) [0xaaabb840b6ec]
 10: (PrimaryLogPG::recover_backfill(unsigned long, ThreadPool::TPHandle&, bool*)+0xc98) [0xaaabb841f218]
 11: (PrimaryLogPG::start_recovery_ops(unsigned long, ThreadPool::TPHandle&, unsigned long*)+0xcc4) [0xaaabb8415d34]
 12: (OSD::do_recovery(PG*, unsigned int, unsigned long, ThreadPool::TPHandle&)+0x228) [0xaaabb8322f48]
 13: (ceph::osd::scheduler::PGRecovery::run(OSD*, OSDShard*, boost::intrusive_ptr<PG>&, ThreadPool::TPHandle&)+0x28) [0xaaabb84a1088]
 14: (OSD::ShardedOpWQ::_process(unsigned int, ceph::heartbeat_handle_d*)+0x63c) [0xaaabb83343c0]
 15: (ShardedThreadPool::shardedthreadpool_worker(unsigned int)+0x23c) [0xaaabb87984ec]
 16: /usr/bin/ceph-osd(+0x8689e8) [0xaaabb87989e8]
 17: /lib64/libc.so.6(+0x82a68) [0xfffbf2ec2a68]
 18: /lib64/libc.so.6(+0x2bb9c) [0xfffbf2e6bb9c]
 NOTE: a copy of the executable, or `objdump -rdS <executable>` is needed to interpret this.


# 函数调用栈




libc.so.6
└── /usr/bin/ceph-osd
    └── ShardedThreadPool::shardedthreadpool_worker
        └── OSD::ShardedOpWQ::_process
            └── ceph::osd::scheduler::PGRecovery::run
                └── OSD::do_recovery
                    └── PrimaryLogPG::start_recovery_ops
                        └── PrimaryLogPG::recover_backfill
                            └── PrimaryLogPG::add_object_context_to_pg_stat
                                └── SnapSet::get_clone_bytes
                                    └── ceph_assert（崩溃）

# 修复pg的命令

()[root@27a16e98cc1e src]# ceph-objectstore-tool --data-path /ceph/ceph/build/out/dev/osd0/ --pgid 3.1a --op fsck --force --no-mon-config
2025-04-28T13:48:35.259+0000 fdb3a1853040 -1 WARNING: all dangerous and experimental features are enabled.
fsck success
()[root@27a16e98cc1e src]# ceph-objectstore-tool --data-path /ceph/ceph/build/out/dev/osd0/ --pgid 3.1a --op repair --force --no-mon-config
repair success
()[root@27a16e98cc1e src]# 

# 在主要 OSD 主机上，先停止 OSD：
ceph-objectstore-tool --data-path /ceph/ceph/build/out/dev/osd0/ --pgid 3.1a --op repair --args '{"corrupt_snapsets": true}'


# 这是正在挂着的两个盘

分别是osd.23和osd.16上的问题


volumes volume-49427900-c946-478a-9c88-71f1674e0c59 rbd_data.17e53ebb72011     # osd.16
volumes volume-290e1e32-694b-404e-a7f0-f01047192d85 rbd_data.5426f358a0edb     # osd.20  -> 已删除

openstack.volume.list.log

```
|ID | Name | Status | Size | Attach to |
-------------------------------------
| 49427900-c946-478a-9c88-71f1674e0c59 | b7426133-0e2f-4815-a16a-1d39af4d6875-blank-vol  | in-use    |   50 | Attached to kylinV10Sp3 on /dev/vda       |
| 290e1e32-694b-404e-a7f0-f01047192d85 | node06_data                                     | in-use    |  500 | Attached to node06 on /dev/vdb            |   -> 删除

```

# VM与volume的映射

 name    node06
id      add20676-ab3a-4541-862a-e1f36c20b00c
volume id    

id='aefd13e7-cf6b-43a9-9f2a-d3c5211424b0'                
id='290e1e32-694b-404e-a7f0-f01047192d85'                 [DEL DONE]
id='90758693-9e2c-41d7-81a7-994ee5fb2f72'                 [DEL DONE]
id='55c4c71b-df2b-473f-8bb9-364b58dea90a' 


name：
name      kylinV10Sp3
id     b7426133-0e2f-4815-a16a-1d39af4d6875
volume id 

id='493035ee-05e2-41d0-9856-5029349ec261'                
id='49427900-c946-478a-9c88-71f1674e0c59'  -> 这个是有snapshot的，需要先把snapshot删除掉

# 首先找到snapshot

openstack volume snapshot list --volume 49427900-c946-478a-9c88-71f1674e0c59



find_prefix.log
55:volumes volume-290e1e32-694b-404e-a7f0-f01047192d85 rbd_data.5426f358a0edb


# 明天要做的事

[1] 把osd.16 osd.20  osd.23上面所有的pg都fsck一把

 ceph-objectstore-tool --data-path /ceph/ceph/build/out/dev/osd0/  --pgid 3.1a --op list
ceph-objectstore-tool --data-path /ceph/ceph/build/out/dev/osd0/ --pgid 3.1a --op fsck
ceph-objectstore-tool --data-path /ceph/ceph/build/out/dev/osd0/ --pgid 3.1a --op repair
ceph-objectstore-tool --data-path /ceph/ceph/build/out/dev/osd0/ --pgid 3.1a --op repair-deep

看一下repair的代码，能不能修复这种情况。

```C++
// bluestore.h

  int repair(bool deep) override {
    return _fsck(deep ? FSCK_DEEP : FSCK_REGULAR, true);
  }

```

# Code

```C++
bool PrimaryLogPG::start_recovery_ops(
  uint64_t max,
  ThreadPool::TPHandle &handle,
  uint64_t *ops_started)
{
  uint64_t& started = *ops_started;
  started = 0;
  bool work_in_progress = false;
  bool recovery_started = false;
  ceph_assert(is_primary());
  ceph_assert(is_peered());
  ceph_assert(!recovery_state.is_deleting());

  ceph_assert(recovery_queued);
  recovery_queued = false;

  if (!state_test(PG_STATE_RECOVERING) &&
      !state_test(PG_STATE_BACKFILLING)) {
    /* TODO: I think this case is broken and will make do_recovery()
     * unhappy since we're returning false */
    dout(10) << "recovery raced and were queued twice, ignoring!" << dendl;
    return have_unfound();
  }

  const auto &missing = recovery_state.get_pg_log().get_missing();

  uint64_t num_unfound = get_num_unfound();

  if (!recovery_state.have_missing()) {
    recovery_state.local_recovery_complete();
  }

  if (!missing.have_missing() || // Primary does not have missing
      // or all of the missing objects are unfound.
      recovery_state.all_missing_unfound()) {
    // Recover the replicas.
    started = recover_replicas(max, handle, &recovery_started);
  }
  if (!started) {
    // We still have missing objects that we should grab from replicas.
    started += recover_primary(max, handle);
  }
  if (!started && num_unfound != get_num_unfound()) {
    // second chance to recovery replicas
    started = recover_replicas(max, handle, &recovery_started);
  }

  if (started || recovery_started)
    work_in_progress = true;

  /* 
   * backfill处理逻辑:
   * 只有在恢复完成后，才考虑执行回填操作
   * 回填是全量数据同步，比recovery更消耗资源
   */
  bool deferred_backfill = false;
  if (recovering.empty() &&             // 没有正在进行的恢复操作
      state_test(PG_STATE_BACKFILLING) &&    // PG处于回填状态
      !get_backfill_targets().empty() && // 有需要回填的目标OSD
      started < max &&                   // 本次函数调用还未达到操作上限
      missing.num_missing() == 0 &&      // 没有丢失对象需要恢复
      waiting_on_backfill.empty()) {     // 没有等待中的回填操作
    
    /* 延迟回填的三种情况 */
    if (get_osdmap()->test_flag(CEPH_OSDMAP_NOBACKFILL)) {
      // 情况1: 集群设置了NOBACKFILL标志，管理员明确禁止所有回填操作
      dout(10) << "deferring backfill due to NOBACKFILL" << dendl;
      deferred_backfill = true;
    } else if (get_osdmap()->test_flag(CEPH_OSDMAP_NOREBALANCE) &&
	       !is_degraded())  {
      // 情况2: 禁止再平衡且PG未降级
      // 只有降级的PG才允许回填，纯再平衡操作会被推迟
      dout(10) << "deferring backfill due to NOREBALANCE" << dendl;
      deferred_backfill = true;
    } else if (!recovery_state.is_backfill_reserved()) {
      /* 情况3: 回填资源尚未预留 - 回填是资源密集型操作，需要预留资源 */
      dout(10) << "deferring backfill due to !backfill_reserved" << dendl;
      if (!backfill_reserving) {
	    // 如果还没开始预留资源，则发送请求预留资源的事件
	    dout(10) << "queueing RequestBackfill" << dendl;
	    backfill_reserving = true;
	    queue_peering_event(
	      PGPeeringEventRef(
	        std::make_shared<PGPeeringEvent>(
	          get_osdmap_epoch(),
	          get_osdmap_epoch(),
	          PeeringState::RequestBackfill())));
      }
      deferred_backfill = true;
    } else {
      // 所有条件满足且没有延迟因素，执行实际回填操作
      // Primary作为协调者，推送数据到需要回填的目标OSD
      started += recover_backfill(max - started, handle, &work_in_progress);
    }
  }

  dout(10) << " started " << started << dendl;
  osd->logger->inc(l_osd_rop, started);

  if (!recovering.empty() ||
      work_in_progress || recovery_ops_active > 0 || deferred_backfill)
    return !work_in_progress && have_unfound();

  ceph_assert(recovering.empty());
  ceph_assert(recovery_ops_active == 0);

  dout(10) << __func__ << " needs_recovery: "
	   << recovery_state.get_missing_loc().get_needs_recovery()
	   << dendl;
  dout(10) << __func__ << " missing_loc: "
	   << recovery_state.get_missing_loc().get_missing_locs()
	   << dendl;
  int unfound = get_num_unfound();
  if (unfound) {
    dout(10) << " still have " << unfound << " unfound" << dendl;
    return true;
  }

  if (missing.num_missing() > 0) {
    // this shouldn't happen!
    osd->clog->error() << info.pgid << " Unexpected Error: recovery ending with "
		       << missing.num_missing() << ": " << missing.get_items();
    return false;
  }

  if (needs_recovery()) {
    // this shouldn't happen!
    // We already checked num_missing() so we must have missing replicas
    osd->clog->error() << info.pgid
                       << " Unexpected Error: recovery ending with missing replicas";
    return false;
  }

  if (state_test(PG_STATE_RECOVERING)) {
    state_clear(PG_STATE_RECOVERING);
    state_clear(PG_STATE_FORCED_RECOVERY);
    if (needs_backfill()) {
      dout(10) << "recovery done, queuing backfill" << dendl;
      queue_peering_event(
        PGPeeringEventRef(
          std::make_shared<PGPeeringEvent>(
            get_osdmap_epoch(),
            get_osdmap_epoch(),
            PeeringState::RequestBackfill())));
    } else {
      dout(10) << "recovery done, no backfill" << dendl;
      state_clear(PG_STATE_FORCED_BACKFILL);
      queue_peering_event(
        PGPeeringEventRef(
          std::make_shared<PGPeeringEvent>(
            get_osdmap_epoch(),
            get_osdmap_epoch(),
            PeeringState::AllReplicasRecovered())));
    }
  } else { // backfilling
    state_clear(PG_STATE_BACKFILLING);
    state_clear(PG_STATE_FORCED_BACKFILL);
    state_clear(PG_STATE_FORCED_RECOVERY);
    dout(10) << "recovery done, backfill done" << dendl;
    queue_peering_event(
      PGPeeringEventRef(
        std::make_shared<PGPeeringEvent>(
          get_osdmap_epoch(),
          get_osdmap_epoch(),
          PeeringState::Backfilled())));
  }

  return false;
}

```

# 功能总结：PrimaryLogPG::start_recovery_ops 方法

这段代码是 Ceph 存储系统中处理数据恢复操作的关键函数，主要负责启动和管理 PG (Placement Group) 的恢复流程。

## 主要功能：
1. **启动恢复操作**：根据当前状态启动适当的恢复过程，包括主节点恢复和副本节点恢复
2. **处理回填操作**：在恢复完成后，根据需要启动回填 (backfill) 操作
3. **管理恢复状态**：跟踪并更新恢复过程中的 PG 状态

## 工作流程：
1. **初始检查**：
   - 验证 PG 是主节点且已完成 peering 过程
   - 确认 PG 当前处于恢复或回填状态

2. **恢复顺序**：
   - 如果主节点没有缺失对象或所有缺失对象都是 unfound：首先尝试恢复副本 (`recover_replicas`)
   - 否则，尝试从副本恢复主节点的缺失对象 (`recover_primary`)
   - 如果 unfound 对象数量变化，再次尝试恢复副本

3. **回填处理**：
   - 当恢复操作完成后，检查是否需要进行回填
   - 根据集群标志 (NOBACKFILL/NOREBALANCE) 决定是否延迟回填
   - 确保回填资源已被预留后，启动回填操作

4. **完成处理**：
   - 清除相应的状态标志 (PG_STATE_RECOVERING 或 PG_STATE_BACKFILLING)
   - 根据需要触发后续事件 (RequestBackfill, AllReplicasRecovered, Backfilled)
   - 检查是否仍有 unfound 对象

## 返回值：
- 返回是否还有 unfound 对象（找不到位置的丢失对象）且当前没有进行中的工作

这个函数是 Ceph 数据恢复机制的核心部分，确保了数据在发生故障后能够被正确地恢复和复制。

# 为什么Primary需要处理Backfill

在Ceph中，Primary节点不仅需要处理Recovery还需要处理Backfill，这是因为它们是两种不同的数据同步机制，而Primary作为PG的协调者负责所有数据一致性操作：

## Recovery vs Backfill

1. **Recovery (恢复)**
   - **增量同步**：基于PG日志记录的变更
   - **适用场景**：OSD短暂离线后重新加入，只需同步有限的最近变更
   - **效率**：更快，消耗资源少

2. **Backfill (回填)**
   - **全量同步**：需要扫描比较PG的所有对象
   - **适用场景**：
     - 新OSD加入集群需要获取完整数据集
     - OSD离线时间过长，日志已不足以跟踪所有变更
     - PG映射发生重大变化（如OSD重新分配）

## Primary的角色

Primary节点作为PG的主协调者：
1. 首先尝试通过Recovery解决数据不一致问题
2. 当Recovery不足以解决问题时（如代码中的`if (needs_backfill())` 条件），需要启动更重的Backfill过程

代码中的处理流程：
```cpp
if (state_test(PG_STATE_RECOVERING)) {
  // Recovery完成
  state_clear(PG_STATE_RECOVERING);
  if (needs_backfill()) {
    // 还需要进行Backfill
    queue_peering_event(PeeringState::RequestBackfill());
  } else {
    // 全部完成
    queue_peering_event(PeeringState::AllReplicasRecovered());
  }
}
```

Primary不是接收Backfill的目标，而是作为整个过程的**协调者和数据源**，负责把正确的数据推送到需要Backfill的副本节点上。

# 主OSD发送数据

所以backfill的意思是说，当前主OSD需要把数据copy到其他的osd吗？

## 是的，Backfill就是主OSD向其他OSD复制数据的过程

在Ceph中，Backfill（回填）的核心含义是：**由Primary OSD负责将完整的PG数据复制到需要全量同步的目标OSD**。

## Backfill vs Recovery

- **Recovery**：增量同步，基于PG日志记录的变更，只同步变更的对象
- **Backfill**：全量同步，需要扫描和比较整个PG的内容，同步所有需要的对象

## Backfill的典型场景

1. 新OSD加入集群时，需要获取它负责的PG数据
2. OSD离线时间太长，日志不足以跟踪所有变更
3. CRUSH映射变化导致PG重新分配
4. 管理员手动更换OSD

## 操作流程

1. Primary OSD作为权威数据源和协调者
2. 扫描自己存储的对象
3. 将这些对象复制到backfill_targets（需要回填的目标OSD）
4. 管理整个回填过程的进度和状态

在代码中的体现就是`recover_backfill()`函数，由Primary调用来执行实际的数据推送操作。

# 第二阶段

```C++

/**
 * recover_backfill
 *
 * Invariants:
 *
 * backfilled: fully pushed to replica or present in replica's missing set (both
 * our copy and theirs).
 *
 * All objects on a backfill_target in
 * [MIN,peer_backfill_info[backfill_target].begin) are valid; logically-removed
 * objects have been actually deleted and all logically-valid objects are replicated.
 * There may be PG objects in this interval yet to be backfilled.
 *
 * All objects in PG in [MIN,backfill_info.begin) have been backfilled to all
 * backfill_targets.  There may be objects on backfill_target(s) yet to be deleted.
 *
 * For a backfill target, all objects < std::min(peer_backfill_info[target].begin,
 *     backfill_info.begin) in PG are backfilled.  No deleted objects in this
 * interval remain on the backfill target.
 *
 * For a backfill target, all objects <= peer_info[target].last_backfill
 * have been backfilled to target
 *
 * There *MAY* be missing/outdated objects between last_backfill_started and
 * std::min(peer_backfill_info[*].begin, backfill_info.begin) in the event that client
 * io created objects since the last scan.  For this reason, we call
 * update_range() again before continuing backfill.
 */
uint64_t PrimaryLogPG::recover_backfill(
  uint64_t max,
  ThreadPool::TPHandle &handle, bool *work_started)
{
  dout(10) << __func__ << " (" << max << ")"
           << " bft=" << get_backfill_targets()
	   << " last_backfill_started " << last_backfill_started
	   << (new_backfill ? " new_backfill":"")
	   << dendl;
  ceph_assert(!get_backfill_targets().empty());

  // Initialize from prior backfill state
  if (new_backfill) {
    // on_activate() was called prior to getting here
    ceph_assert(last_backfill_started == recovery_state.earliest_backfill());
    new_backfill = false;

    // initialize BackfillIntervals
    for (set<pg_shard_t>::const_iterator i = get_backfill_targets().begin();
	 i != get_backfill_targets().end();
	 ++i) {
      peer_backfill_info[*i].reset(
	recovery_state.get_peer_info(*i).last_backfill);
    }
    backfill_info.reset(last_backfill_started);

    backfills_in_flight.clear();
    pending_backfill_updates.clear();
  }

  for (set<pg_shard_t>::const_iterator i = get_backfill_targets().begin();
       i != get_backfill_targets().end();
       ++i) {
    dout(10) << "peer osd." << *i
	   << " info " << recovery_state.get_peer_info(*i)
	   << " interval " << peer_backfill_info[*i].begin
	   << "-" << peer_backfill_info[*i].end
	   << " " << peer_backfill_info[*i].objects.size() << " objects"
	   << dendl;
  }

  // update our local interval to cope with recent changes
  backfill_info.begin = last_backfill_started;
  update_range(&backfill_info, handle);

  unsigned ops = 0;
  vector<boost::tuple<hobject_t, eversion_t, pg_shard_t> > to_remove;
  set<hobject_t> add_to_stat;

  for (set<pg_shard_t>::const_iterator i = get_backfill_targets().begin();
       i != get_backfill_targets().end();
       ++i) {
    peer_backfill_info[*i].trim_to(
      std::max(
	recovery_state.get_peer_info(*i).last_backfill,
	last_backfill_started));
  }
  backfill_info.trim_to(last_backfill_started);

  PGBackend::RecoveryHandle *h = pgbackend->open_recovery_op();
  while (ops < max) {
    if (backfill_info.begin <= earliest_peer_backfill() &&
	!backfill_info.extends_to_end() && backfill_info.empty()) {
      hobject_t next = backfill_info.end;
      backfill_info.reset(next);
      backfill_info.end = hobject_t::get_max();
      update_range(&backfill_info, handle);
      backfill_info.trim();
    }

    dout(20) << "   my backfill interval " << backfill_info << dendl;

    bool sent_scan = false;
    for (set<pg_shard_t>::const_iterator i = get_backfill_targets().begin();
	 i != get_backfill_targets().end();
	 ++i) {
      pg_shard_t bt = *i;
      BackfillInterval& pbi = peer_backfill_info[bt];

      dout(20) << " peer shard " << bt << " backfill " << pbi << dendl;
      if (pbi.begin <= backfill_info.begin &&
	  !pbi.extends_to_end() && pbi.empty()) {
	dout(10) << " scanning peer osd." << bt << " from " << pbi.end << dendl;
	epoch_t e = get_osdmap_epoch();
	MOSDPGScan *m = new MOSDPGScan(
	  MOSDPGScan::OP_SCAN_GET_DIGEST, pg_whoami, e, get_last_peering_reset(),
	  spg_t(info.pgid.pgid, bt.shard),
	  pbi.end, hobject_t());
	osd->send_message_osd_cluster(bt.osd, m, get_osdmap_epoch());
	ceph_assert(waiting_on_backfill.find(bt) == waiting_on_backfill.end());
	waiting_on_backfill.insert(bt);
        sent_scan = true;
      }
    }

    // Count simultaneous scans as a single op and let those complete
    if (sent_scan) {
      ops++;
      start_recovery_op(hobject_t::get_max()); // XXX: was pbi.end
      break;
    }

    if (backfill_info.empty() && all_peer_done()) {
      dout(10) << " reached end for both local and all peers" << dendl;
      break;
    }

    // Get object within set of peers to operate on and
    // the set of targets for which that object applies.
    hobject_t check = earliest_peer_backfill();

    if (check < backfill_info.begin) {

      set<pg_shard_t> check_targets;
      for (set<pg_shard_t>::const_iterator i = get_backfill_targets().begin();
	   i != get_backfill_targets().end();
	   ++i) {
        pg_shard_t bt = *i;
        BackfillInterval& pbi = peer_backfill_info[bt];
        if (pbi.begin == check)
          check_targets.insert(bt);
      }
      ceph_assert(!check_targets.empty());

      dout(20) << " BACKFILL removing " << check
	       << " from peers " << check_targets << dendl;
      for (set<pg_shard_t>::iterator i = check_targets.begin();
	   i != check_targets.end();
	   ++i) {
        pg_shard_t bt = *i;
        BackfillInterval& pbi = peer_backfill_info[bt];
        ceph_assert(pbi.begin == check);

        to_remove.push_back(boost::make_tuple(check, pbi.objects.begin()->second, bt));
        pbi.pop_front();
      }

      last_backfill_started = check;

      // Don't increment ops here because deletions
      // are cheap and not replied to unlike real recovery_ops,
      // and we can't increment ops without requeueing ourself
      // for recovery.
    } else {
      eversion_t& obj_v = backfill_info.objects.begin()->second;

      vector<pg_shard_t> need_ver_targs, missing_targs, keep_ver_targs, skip_targs;
      for (set<pg_shard_t>::const_iterator i = get_backfill_targets().begin();
	   i != get_backfill_targets().end();
	   ++i) {
	pg_shard_t bt = *i;
	BackfillInterval& pbi = peer_backfill_info[bt];
        // Find all check peers that have the wrong version
	if (check == backfill_info.begin && check == pbi.begin) {
	  if (pbi.objects.begin()->second != obj_v) {
	    need_ver_targs.push_back(bt);
	  } else {
	    keep_ver_targs.push_back(bt);
	  }
        } else {
	  const pg_info_t& pinfo = recovery_state.get_peer_info(bt);

          // Only include peers that we've caught up to their backfill line
	  // otherwise, they only appear to be missing this object
	  // because their pbi.begin > backfill_info.begin.
          if (backfill_info.begin > pinfo.last_backfill)
	    missing_targs.push_back(bt);
	  else
	    skip_targs.push_back(bt);
	}
      }

      if (!keep_ver_targs.empty()) {
        // These peers have version obj_v
	dout(20) << " BACKFILL keeping " << check
		 << " with ver " << obj_v
		 << " on peers " << keep_ver_targs << dendl;
	//assert(!waiting_for_degraded_object.count(check));
      }
      if (!need_ver_targs.empty() || !missing_targs.empty()) {
	ObjectContextRef obc = get_object_context(backfill_info.begin, false);
	ceph_assert(obc);
	if (obc->get_recovery_read()) {
	  if (!need_ver_targs.empty()) {
	    dout(20) << " BACKFILL replacing " << check
		   << " with ver " << obj_v
		   << " to peers " << need_ver_targs << dendl;
	  }
	  if (!missing_targs.empty()) {
	    dout(20) << " BACKFILL pushing " << backfill_info.begin
	         << " with ver " << obj_v
	         << " to peers " << missing_targs << dendl;
	  }
	  vector<pg_shard_t> all_push = need_ver_targs;
	  all_push.insert(all_push.end(), missing_targs.begin(), missing_targs.end());

	  handle.reset_tp_timeout();
	  int r = prep_backfill_object_push(backfill_info.begin, obj_v, obc, all_push, h);
	  if (r < 0) {
	    *work_started = true;
	    dout(0) << __func__ << " Error " << r << " trying to backfill " << backfill_info.begin << dendl;
	    break;
	  }
	  ops++;
	} else {
	  *work_started = true;
	  dout(20) << "backfill blocking on " << backfill_info.begin
		   << "; could not get rw_manager lock" << dendl;
	  break;
	}
      }
      dout(20) << "need_ver_targs=" << need_ver_targs
	       << " keep_ver_targs=" << keep_ver_targs << dendl;
      dout(20) << "backfill_targets=" << get_backfill_targets()
	       << " missing_targs=" << missing_targs
	       << " skip_targs=" << skip_targs << dendl;

      last_backfill_started = backfill_info.begin;
      add_to_stat.insert(backfill_info.begin); // XXX: Only one for all pushes?
      backfill_info.pop_front();
      vector<pg_shard_t> check_targets = need_ver_targs;
      check_targets.insert(check_targets.end(), keep_ver_targs.begin(), keep_ver_targs.end());
      for (vector<pg_shard_t>::iterator i = check_targets.begin();
	   i != check_targets.end();
	   ++i) {
        pg_shard_t bt = *i;
        BackfillInterval& pbi = peer_backfill_info[bt];
        pbi.pop_front();
      }
    }
  }

  for (set<hobject_t>::iterator i = add_to_stat.begin();
       i != add_to_stat.end();
       ++i) {
    ObjectContextRef obc = get_object_context(*i, false);
    ceph_assert(obc);
    pg_stat_t stat;
    add_object_context_to_pg_stat(obc, &stat);
    pending_backfill_updates[*i] = stat;
  }
  map<pg_shard_t,MOSDPGBackfillRemove*> reqs;
  for (unsigned i = 0; i < to_remove.size(); ++i) {
    handle.reset_tp_timeout();
    const hobject_t& oid = to_remove[i].get<0>();
    eversion_t v = to_remove[i].get<1>();
    pg_shard_t peer = to_remove[i].get<2>();
    MOSDPGBackfillRemove *m;
    auto it = reqs.find(peer);
    if (it != reqs.end()) {
      m = it->second;
    } else {
      m = reqs[peer] = new MOSDPGBackfillRemove(
	spg_t(info.pgid.pgid, peer.shard),
	get_osdmap_epoch());
    }
    m->ls.push_back(make_pair(oid, v));

    if (oid <= last_backfill_started)
      pending_backfill_updates[oid]; // add empty stat!
  }
  for (auto p : reqs) {
    osd->send_message_osd_cluster(p.first.osd, p.second,
				  get_osdmap_epoch());
  }

  pgbackend->run_recovery_op(h, get_recovery_op_priority());

  hobject_t backfill_pos =
    std::min(backfill_info.begin, earliest_peer_backfill());
  dout(5) << "backfill_pos is " << backfill_pos << dendl;
  for (set<hobject_t>::iterator i = backfills_in_flight.begin();
       i != backfills_in_flight.end();
       ++i) {
    dout(20) << *i << " is still in flight" << dendl;
  }

  hobject_t next_backfill_to_complete = backfills_in_flight.empty() ?
    backfill_pos : *(backfills_in_flight.begin());
  hobject_t new_last_backfill = recovery_state.earliest_backfill();
  dout(10) << "starting new_last_backfill at " << new_last_backfill << dendl;
  for (map<hobject_t, pg_stat_t>::iterator i =
	 pending_backfill_updates.begin();
       i != pending_backfill_updates.end() &&
	 i->first < next_backfill_to_complete;
       pending_backfill_updates.erase(i++)) {
    dout(20) << " pending_backfill_update " << i->first << dendl;
    ceph_assert(i->first > new_last_backfill);
    // carried from a previous round – if we are here, then we had to
    // be requeued (by e.g. on_global_recover()) and those operations
    // are done.
    recovery_state.update_complete_backfill_object_stats(
      i->first,
      i->second);
    new_last_backfill = i->first;
  }
  dout(10) << "possible new_last_backfill at " << new_last_backfill << dendl;

  ceph_assert(!pending_backfill_updates.empty() ||
	 new_last_backfill == last_backfill_started);
  if (pending_backfill_updates.empty() &&
      backfill_pos.is_max()) {
    ceph_assert(backfills_in_flight.empty());
    new_last_backfill = backfill_pos;
    last_backfill_started = backfill_pos;
  }
  dout(10) << "final new_last_backfill at " << new_last_backfill << dendl;

  // If new_last_backfill == MAX, then we will send OP_BACKFILL_FINISH to
  // all the backfill targets.  Otherwise, we will move last_backfill up on
  // those targets need it and send OP_BACKFILL_PROGRESS to them.
  for (set<pg_shard_t>::const_iterator i = get_backfill_targets().begin();
       i != get_backfill_targets().end();
       ++i) {
    pg_shard_t bt = *i;
    const pg_info_t& pinfo = recovery_state.get_peer_info(bt);

    if (new_last_backfill > pinfo.last_backfill) {
      recovery_state.update_peer_last_backfill(bt, new_last_backfill);
      epoch_t e = get_osdmap_epoch();
      MOSDPGBackfill *m = NULL;
      if (pinfo.last_backfill.is_max()) {
        m = new MOSDPGBackfill(
	  MOSDPGBackfill::OP_BACKFILL_FINISH,
	  e,
	  get_last_peering_reset(),
	  spg_t(info.pgid.pgid, bt.shard));
        // Use default priority here, must match sub_op priority
        start_recovery_op(hobject_t::get_max());
      } else {
        m = new MOSDPGBackfill(
	  MOSDPGBackfill::OP_BACKFILL_PROGRESS,
	  e,
	  get_last_peering_reset(),
	  spg_t(info.pgid.pgid, bt.shard));
        // Use default priority here, must match sub_op priority
      }
      m->last_backfill = pinfo.last_backfill;
      m->stats = pinfo.stats;
      osd->send_message_osd_cluster(bt.osd, m, get_osdmap_epoch());
      dout(10) << " peer " << bt
	       << " num_objects now " << pinfo.stats.stats.sum.num_objects
	       << " / " << info.stats.stats.sum.num_objects << dendl;
    }
  }

  if (ops)
    *work_started = true;
  return ops;
}
```

# Step 3

我看到这个函数在调用`add_object_context_to_pg_stat`。这个add_object_context_to_pg_stat函数的作用是什么？为什么需要用到它？ 


# add_object_context_to_pg_stat 函数作用

`add_object_context_to_pg_stat` 函数的作用是**将对象的元数据和统计信息添加到 PG 的统计数据结构中**。在回填过程中使用它的原因与保持集群统计信息的准确性和一致性有关。

## 具体作用

1. **提取对象元数据**：从对象上下文(ObjectContext)中提取关键统计信息，包括：
   - 对象大小
   - 对象版本
   - 所占存储空间
   - 对象类型（普通、克隆、删除标记等）

2. **更新统计数据**：将这些信息整合到传入的 `pg_stat_t` 结构中，用于跟踪 PG 的整体统计指标。

## 使用原因

在 `recover_backfill` 函数中，这个函数出现在如下代码段中：
```cpp
for (set<hobject_t>::iterator i = add_to_stat.begin();
     i != add_to_stat.end();
     ++i) {
  ObjectContextRef obc = get_object_context(*i, false);
  ceph_assert(obc);
  pg_stat_t stat;
  add_object_context_to_pg_stat(obc, &stat);
  pending_backfill_updates[*i] = stat;
}
```

需要这样做的原因是：

1. **统计一致性**：确保在回填完成后，所有 OSD（包括主节点和回填目标）上对 PG 内容的统计信息保持一致。

2. **通知机制**：稍后这些统计信息会通过 `MOSDPGBackfill` 消息发送给回填目标：
   ```cpp
   m->stats = pinfo.stats;  // 包含了更新后的统计信息
   osd->send_message_osd_cluster(bt.osd, m, get_osdmap_epoch());
   ```

3. **管理决策基础**：这些准确的统计信息将被用于：
   - 集群负载均衡决策
   - 存储利用率监控
   - 数据分布报告
   - 性能指标计算

简单来说，这个函数确保了当对象被回填到目标 OSD 时，这些 OSD 不仅获得了对象数据，还获得了准确的统计元数据，从而保持了整个集群视图的一致性。

# 4 获取快照集出错


```C++
void PrimaryLogPG::add_object_context_to_pg_stat(ObjectContextRef obc, pg_stat_t *pgstat)
{
  object_info_t& oi = obc->obs.oi;  // 获取对象信息

  dout(10) << __func__ << " " << oi.soid << dendl;
  ceph_assert(!oi.soid.is_snapdir());  // 确保不是snapdir对象

  // 创建临时统计结构
  object_stat_sum_t stat;
  
  // 基本对象计数
  stat.num_objects++;
  
  // 各种特殊对象类型计数
  if (oi.is_dirty())
    stat.num_objects_dirty++;    // 脏对象（有未刷盘的写入）
  if (oi.is_whiteout())
    stat.num_whiteouts++;        // 删除标记对象
  if (oi.is_omap())
    stat.num_objects_omap++;     // 使用对象映射的对象
  if (oi.is_cache_pinned())
    stat.num_objects_pinned++;   // 缓存锁定的对象
  if (oi.has_manifest())
    stat.num_objects_manifest++; // 有清单的对象

  // 快照对象特殊处理
  if (oi.soid.is_snap()) {
    stat.num_object_clones++;    // 增加克隆计数

    // 确保有快照集上下文
    if (!obc->ssc)
      obc->ssc = get_snapset_context(oi.soid, false);
    ceph_assert(obc->ssc);
    
    // 从快照集获取此快照的大小
    stat.num_bytes += obc->ssc->snapset.get_clone_bytes(oi.soid.snap);
  } else {
    // 普通对象直接使用其大小
    stat.num_bytes += oi.size;
  }

  // 将临时统计添加到PG统计中
  pgstat->stats.sum.add(stat);
}

```

# Step 4

SnapSet::get_clone_bytes 函数解析

```
# SnapSet::get_clone_bytes 函数解析

这个函数计算**特定快照克隆实际占用的唯一存储空间**，是Ceph写时复制(COW)快照系统的核心计算逻辑。

## 代码详解

```cpp
uint64_t SnapSet::get_clone_bytes(snapid_t clone) const {
    ceph_assert(clone_size.count(clone));
    uint64_t size = clone_size.find(clone)->second;    // 获取克隆的总逻辑大小
    
    ceph_assert(clone_overlap.count(clone));
    const interval_set<uint64_t> &overlap = clone_overlap.find(clone)->second;  // 获取与其他版本重叠的区间
    
    ceph_assert(size >= (uint64_t)overlap.size());
    return size - overlap.size();  // 返回实际唯一占用空间 = 总大小 - 重叠大小
}
```

## 关键概念

1. **clone_size**：
   - 记录每个快照克隆的完整逻辑大小
   - 表示如果这是一个独立对象会占用多少空间

2. **clone_overlap**：
   - 记录每个快照克隆与其他版本(通常是head对象)共享的数据区间
   - 使用`interval_set`类型存储，可以表示多个不连续的数据区间

3. **实际占用空间计算**：
   - 实际额外存储 = 总逻辑大小 - 共享数据大小
   - 这反映了COW机制下只存储差异部分的优化

## 工作原理举例

假设一个10MB的对象：
1. 创建快照A，此时快照A与head完全共享数据
   - clone_size[A] = 10MB
   - clone_overlap[A] = 10MB
   - get_clone_bytes(A) = 10MB - 10MB = 0MB

2. 修改head对象的前2MB
   - clone_size[A] = 10MB
   - clone_overlap[A] = 8MB (后8MB仍共享)
   - get_clone_bytes(A) = 10MB - 8MB = 2MB

通过这个计算，Ceph可以准确统计每个快照实际占用的物理存储空间，避免在统计时重复计算共享数据，从而提供精确的存储使用情况报告。
```