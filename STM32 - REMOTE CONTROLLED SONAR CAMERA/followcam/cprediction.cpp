#include "cprediction.h"

CPrediction::CPrediction(int label, double confidence):m_label(label),m_confidence(confidence)
{
}
 bool CPrediction:: check_confidence(int threshold)
{
    return (m_confidence < threshold);
}
 int CPrediction:: get_label()
{
    return m_label;
}
double CPrediction::get_confidence()
{
    return m_confidence;
}
 void CPrediction::set_confidence(double confidence)
{
    m_confidence = confidence;
}
 void CPrediction::set_label(int label)
{
    m_label = label;
}
