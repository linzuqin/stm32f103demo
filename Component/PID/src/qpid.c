/*
 * qpid.c
 *
 * Change Logs:
 * Date           Author            Notes
 * 2021-09-09     qiyongzhong       first version
 * 2025-03-10     wangrongwen       add integral anti-windup
 */

#include <qpid.h>
#include "sys.h"

#if		USE_PID

#define QPID_ASSERT(x) RT_ASSERT(x)

void qpid_init(qpid_t *qpid) // 初始化qpid
{
    QPID_ASSERT(qpid != RT_NULL);
    memset(qpid, 0, sizeof(qpid_t));
}

void qpid_set_dst(qpid_t *qpid, float dst) // 设置目标值
{
    QPID_ASSERT(qpid != RT_NULL);
    qpid->dst = dst;
}

void qpid_set_ratio(qpid_t *qpid, float kp, float ki, float kd) // 设置各项比例
{
    QPID_ASSERT(qpid != RT_NULL);
    qpid->kp = kp;
    qpid->ki = ki;
    qpid->kd = kd;
}

void qpid_set_ki_lmt(qpid_t *qpid, float ki_max, float ki_min) // 设置积分限值
{
    QPID_ASSERT(qpid != RT_NULL);
    qpid->ki_max = ki_max;
    qpid->ki_min = ki_min;
}

void qpid_set_out_lmt(qpid_t *qpid, float out_min, float out_max) // 设置输出限值
{
    QPID_ASSERT(qpid != RT_NULL);
    qpid->out_min = out_min;
    qpid->out_max = out_max;
}

float qpid_cal_inc(qpid_t *qpid, float cur) // 计算增量型pid, 输出增量值
{
    float rst = 0;

    QPID_ASSERT(qpid != RT_NULL);

    qpid->err[2] = qpid->err[1];    // 转移上上次偏差到err[2]
    qpid->err[1] = qpid->err[0];    // 转移上次偏差到err[1]
    qpid->err[0] = qpid->dst - cur; // 计算本次偏差到err[0]

    rst = qpid->kp * (qpid->err[0] - qpid->err[1]);                       // 计算比例项
    rst += qpid->ki * qpid->err[0];                                       // 计算累加积分项
    rst += qpid->kd * (qpid->err[0] - (qpid->err[1] * 2) + qpid->err[2]); // 计算累加微分项

    if ((qpid->out_min * qpid->out_max >= 0) || (qpid->out_min >= qpid->out_max)) // 限值参数不可用
    {
        return (rst);
    }

    if (rst < qpid->out_min)
    {
        return (qpid->out_min);
    }

    if (rst > qpid->out_max)
    {
        return (qpid->out_max);
    }

    return (rst);
}

float qpid_cal_pos(qpid_t *qpid, float cur) // 计算位置型pid, 输出位置值
{
    float rst = 0;

    QPID_ASSERT(qpid != RT_NULL);

    // 更新偏差，先将上一次偏差赋值，然后计算当前偏差
    qpid->err[1] = qpid->err[0];
    qpid->err[0] = qpid->dst - cur;

    // 计算比例项和微分项
    rst = qpid->kp * qpid->err[0];
    rst += qpid->kd * (qpid->err[0] - qpid->err[1]);

#ifdef ENABLE_INTEGRAL_ANTI_WINDUP
    // 开启积分抗饱和处理
    if (qpid->ki_min < qpid->ki_max) // 限值参数可用
    {
        // 条件积分：只有当前输出未饱和时才更新积分项
        if ((rst < qpid->out_max) && (rst > qpid->out_min))
        {
            qpid->err[2] += qpid->err[0];
            // 限幅处理，防止积分项过大
            if (qpid->err[2] > qpid->ki_max)
            {
                qpid->err[2] = qpid->ki_max;
            }
            else if (qpid->err[2] < qpid->ki_min)
            {
                qpid->err[2] = qpid->ki_min;
            }
        }
    }
    else
    {
        qpid->err[2] += qpid->err[0];
    }
#else
    // 不开启抗饱和处理时，直接累计积分项
    qpid->err[2] += qpid->err[0];
#endif

    // 计算积分项并累加到输出
    rst += qpid->ki * qpid->err[2];

    // 对输出进行限幅处理
    if (qpid->out_min < qpid->out_max)
    {
        if (rst < qpid->out_min)
        {
            rst = qpid->out_min;
        }
        else if (rst > qpid->out_max)
        {
            rst = qpid->out_max;
        }
    }

    return rst;
}

#endif
