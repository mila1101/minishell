#include "libft.h"



t_list	*ft_lst_newlist(void *content)
{
	t_list *s1;

	s1 = (t_list *)malloc(sizeof(t_list));
	if (!s1)
		return (NULL);
	s1->content = content;
	s1->next = NULL;
	return (s1);
}